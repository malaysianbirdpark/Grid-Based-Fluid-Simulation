struct PS_IN {
    float3 world_pos : POSITION;
    float3 uvw       : TEXCOORD;
    float4 sv_pos    : SV_Position;
};

Texture3D<min16float4> volume_tex : register(t0);

Texture2D<min16float3> front_texcoord : register(t2);
Texture2D<min16float3> front_world    : register(t3);
Texture2D<min16float3> back_texcoord  : register(t4);
Texture2D<min16float3> back_world     : register(t5);

Texture1D<min16float4> jitter    : register(t6);

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
    [unroll]
    for (int i = 0; i != 20; ++i) {
        const min16float src = volume_tex.Sample(sampler0, origin).r;
        visibility *= exp(-(coeff * src) * step_size);
        origin     += dir * step_size;
    }
    return visibility;
}

min16float HenyeyGreenstein(min16float cos) {
    static const min16float g = 0.0f;
    static const min16float coeff = 1.0f / (4.0f * 3.148592f);
    const min16float denom = 1.0f + g * g - 2.0f * g * cos;
    return coeff * (1.0f - g * g) / (denom * sqrt(denom));
}

float4 main(PS_IN input) : SV_Target
{
	static const min16float3 dr = min16float3(reciprocal_width, reciprocal_height, reciprocal_depth) * 0.5f;

    const min16float3 pixel_pos = input.sv_pos.xyz + min16float3(0.5f, 0.5f, 0.0f);

    const min16float3 front_uvw = front_texcoord.Load(pixel_pos);
    const min16float3 back_uvw  = back_texcoord.Load(pixel_pos);
    const min16float  uvw_len   = length(back_uvw - front_uvw);
    const min16float3 uvw_dir   = (back_uvw - front_uvw) / uvw_len;

    const min16float step_size = reciprocal_width * clamp(uvw_len, 1.0f, 2.0f);
    const min16float3 step_uvw = uvw_dir * step_size;

    if (uvw_len < step_size)
        return min16float4(0.0f, 0.0f, 0.0f, 0.0f);

    const min16float3 front_pos = front_world.Load(pixel_pos);
    const min16float3 back_pos  = back_world.Load(pixel_pos);
    const min16float  world_len = length(back_pos - front_pos);
    const min16float3 world_dir = (back_pos - front_pos) / world_len;
    
    min16float3 cur_uvw = front_uvw + 1e-4;
    min16float3 cur_world = front_pos + 1e-4;

    static const min16float3 albedo = min16float3(10.0f, 10.0f, 10.0f);

    float4 dest_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    min16float src = 0.0f;

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

        src = (val[0] + val[1] + val[2] + val[3] + val[4] + val[5] + val[6] + val[7]) * 0.125f;

        static const min16float absorption_coeff = 0.2f;
        static const min16float scattering_coeff = 0.1f;
        if (src >= 0.9f) 
        {
            min16float3 dir_to_light = pl_pos - cur_world;
			const min16float dist_to_light = length(dir_to_light);
			const min16float dist_to_light_norm = 1.0f - saturate(dist_to_light * pl_reciprocal_range);
			const min16float att = dist_to_light_norm * dist_to_light_norm;
            dir_to_light = dir_to_light / dist_to_light;
            dir_to_light.y = -dir_to_light.y;
            const min16float light_visibility = LightVisibility(cur_uvw, scattering_coeff, dir_to_light, step_size);
            const min16float phase = HenyeyGreenstein(dot(dir_to_light, world_dir));

            const min16float prev_visibility = dest_color.a;
            const min16float coeff = src * (absorption_coeff + scattering_coeff);
            dest_color.a   *= exp(-coeff * step_size);
            const min16float absorption = max(prev_visibility - dest_color.a, 0.0f);
            dest_color.rgb += absorption * albedo * phase * pl_color * light_visibility * att;
        }

        cur_uvw   += step_uvw;
        cur_world += step_world;
        jit = (jit + 1) - ((jit + 1) & 16);

        if (dest_color.a <= 1e-3)
            break;
    }

    dest_color.a = 1.0f - dest_color.a;
    return dest_color;
}