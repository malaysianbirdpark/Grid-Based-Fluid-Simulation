struct PS_IN {
    float3 world_pos : POSITION;
    float3 uvw       : TEXCOORD;
    float4 sv_pos    : SV_Position;
};

Texture3D<min16float3> volume_tex : register(t0);

Texture2D<min16float3> front_texcoord : register(t1);
Texture2D<min16float3> front_world    : register(t2);
Texture2D<min16float3> back_texcoord  : register(t3);
Texture2D<min16float3> back_world     : register(t4);

Texture1D<min16float4> jitter    : register(t5);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);

cbuffer PointLight : register(b2)
{
    min16float3 pl_pos;
    min16float3 pl_color;
    min16float  pl_reciprocal_range;
}

cbuffer Dimension : register(b3) 
{
    uint width;
    uint height;
    uint depth;
    float reciprocal_width;
    float reciprocal_height;
    float reciprocal_depth;
}

min16float LightVisibility(min16float3 origin, min16float coeff, min16float3 dir, min16float step_size) {
    min16float visibility = 1.0f;
    for (int i = 0; i != 15; ++i) {
        const min16float src = volume_tex.Sample(sampler0, origin).r;
        visibility *= exp(-(coeff * src) * step_size);
        origin     += dir * step_size;
    }
    return visibility;
}

min16float3 GetColor(min16float t) {
    min16float3 result = min16float3(0.0f, 0.0f, 0.0f);

    t = clamp(t, 1000.0f, 40000.0f);
    t /= 100.0f;

    if (t <= 66.0f) {
        result.r = 255.0f;
    }
    else {
        result.r = t - 60.0f;
        result.r = 329.698727446f * pow(result.r, -0.1332047592f);
        result.r = clamp(result.r, 0.0f, 255.0f);
    }

    if (t <= 66.0f) {
        result.g = t;
        result.g = 99.4708025861f * log(result.g) - 161.1195681661f;
        result.g = clamp(result.g, 0.0f, 255.0f);
    }
    else {
        result.g = t - 60.0f;
        result.g = 288.1221695283f * pow(result.g, -0.0755148492f);
        result.g = clamp(result.g, 0.0f, 255.0f);
    }

    if (t >= 66.0f) {
        result.b = 255.0f;
    }
    else {
        result.b = t - 10.0f;
        result.b = 138.5177312231f * log(result.b) - 305.0447927307f;
        result.b = clamp(result.b, 0.0f, 255.0f);
    }

    return result * 0.003921568f;
}

float4 main(PS_IN input) : SV_Target
{
	static const min16float3 dr = min16float3(reciprocal_width, reciprocal_height, reciprocal_depth) * 0.5f;

    const min16float3 pixel_pos = input.sv_pos.xyz + min16float3(0.5f, 0.5f, 0.0f);

    const min16float3 front_uvw = front_texcoord.Load(pixel_pos);
    const min16float3 back_uvw  = back_texcoord.Load(pixel_pos);
    const min16float  uvw_len   = length(back_uvw - front_uvw);
    const min16float3 uvw_dir   = (back_uvw - front_uvw) / uvw_len;

    const min16float3 front_pos = front_world.Load(pixel_pos);
    const min16float3 back_pos  = back_world.Load(pixel_pos);
    const min16float  world_len = length(back_pos - front_pos);
    const min16float3 world_dir = (back_pos - front_pos) / world_len;
    
    min16float3 cur_uvw = front_uvw;
    min16float3 cur_world = front_pos;

    static const min16float3 albedo = min16float3(0.3f, 0.3f, 0.3f);

    float4 dest_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    min16float src = 0.0f;

    static const min16float step_size = reciprocal_width;
    const min16float3 step_uvw = uvw_dir * step_size;

    if (uvw_len < step_size)
        return min16float4(0.0f, 0.0f, 0.0f, 0.0f);

    const int iterations = uvw_len / step_size;
    const min16float step_world = world_len / iterations;

    static uint jit = 0;

    [loop]
    for (int i = 0; i < iterations + 1; ++i) { 
        const min16float3 jitt = jitter[jit];

        const min16float3 p0 = cur_uvw + min16float3( dr.x,  dr.y,  dr.z) + min16float3(jitt.x, jitt.y, jitt.z);
        const min16float3 p1 = cur_uvw + min16float3(-dr.x,  dr.y,  dr.z) + min16float3(jitt.y, jitt.z, jitt.x);
        const min16float3 p2 = cur_uvw + min16float3( dr.x, -dr.y,  dr.z) + min16float3(jitt.z, jitt.x, jitt.y);
        const min16float3 p3 = cur_uvw + min16float3(-dr.x, -dr.y,  dr.z) + min16float3(jitt.x, jitt.y, jitt.z);
        const min16float3 p4 = cur_uvw + min16float3( dr.x,  dr.y, -dr.z) + min16float3(jitt.y, jitt.z, jitt.x);
        const min16float3 p5 = cur_uvw + min16float3(-dr.x,  dr.y, -dr.z) + min16float3(jitt.z, jitt.x, jitt.y);
        const min16float3 p6 = cur_uvw + min16float3( dr.x, -dr.y, -dr.z) + min16float3(jitt.x, jitt.y, jitt.z);
        const min16float3 p7 = cur_uvw + min16float3(-dr.x, -dr.y, -dr.z) + min16float3(jitt.y, jitt.z, jitt.x);

        min16float val[8];
        val[0] = volume_tex.Sample(sampler0, p0).r;
        val[1] = volume_tex.Sample(sampler0, p1).r;
        val[2] = volume_tex.Sample(sampler0, p2).r;
        val[3] = volume_tex.Sample(sampler0, p3).r;
        val[4] = volume_tex.Sample(sampler0, p4).r;
        val[5] = volume_tex.Sample(sampler0, p5).r;
        val[6] = volume_tex.Sample(sampler0, p6).r;
        val[7] = volume_tex.Sample(sampler0, p7).r;

        min16float int_val[4];
        int_val[0] = lerp(val[0], val[1], 0.5f);
        int_val[1] = lerp(val[2], val[3], 0.5f);
        const min16float int_val_0 = lerp(int_val[0], int_val[1], 0.5f);
        int_val[2] = lerp(val[4], val[5], 0.5f);
        int_val[3] = lerp(val[6], val[7], 0.5f);
        const min16float int_val_1 = lerp(int_val[2], int_val[3], 0.5f);
        src = lerp(int_val_0, int_val_1, 0.5f);

        const min16float temperature = volume_tex.Sample(sampler0, cur_uvw).g;

        static const min16float absorption_coeff = 0.35f;
        if (src > 1e-3) 
        {
            min16float3 dir_to_light = pl_pos - cur_world;
			const min16float dist_to_light = length(dir_to_light);
			const min16float dist_to_light_norm = 1.0f - saturate(dist_to_light * pl_reciprocal_range);
			const min16float att = dist_to_light_norm * dist_to_light_norm;
            dir_to_light = dir_to_light / dist_to_light;
            dir_to_light.y = -dir_to_light.y;
            const min16float light_visibility = LightVisibility(cur_uvw, absorption_coeff, dir_to_light, step_size);

            const min16float prev_visibility = dest_color.a;
            const min16float coeff = src * absorption_coeff;
            dest_color.a   *= exp(-coeff * step_size);
            const min16float absorption = prev_visibility - dest_color.a;
            dest_color.rgb += absorption * GetColor(temperature) * pl_color * light_visibility * att;
        }

        if (dest_color.a <= 1e-4)
            break;

        cur_uvw   += step_uvw;
        cur_world += step_world;
        jit = (jit + 1) - ((jit + 1) & 16);
    }

    dest_color.a = 1.0f - dest_color.a;
    return dest_color;
}