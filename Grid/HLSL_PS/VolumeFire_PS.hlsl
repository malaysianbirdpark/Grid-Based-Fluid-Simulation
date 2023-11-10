struct PS_IN {
    float4 world_pos  : POSITION;
    float3 uvw        : TEXCOORD;
    float4 sv_pos     : SV_Position;
};

Texture3D<min16float3> volume_tex : register(t0);

Texture2D front_texcoord : register(t1);
Texture2D back_texcoord  : register(t2);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);

float4 main(PS_IN input) : SV_Target
{
    uint width;
    uint height;
    uint depth;
    volume_tex.GetDimensions(width, height, depth);

    const float3 dr = float3((0.5f / width), (0.5f / height), (0.5f / depth));

    const float3 pixel_pos = input.sv_pos.xyz + float3(0.5f, 0.5f, 0.0f);
    const float3 front_uvw = front_texcoord.Load(input.sv_pos);
    const float3 back_uvw  = back_texcoord.Load(input.sv_pos);
    const float  uvw_len   = length(back_uvw - front_uvw);
    const float3 uvw_dir   = (back_uvw - front_uvw) / uvw_len;
    
    float3 cur_uvw = front_uvw;

    static const int iterations = 50;

    float4 dest_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float src = 0.0f;

    const float step_size = uvw_len / iterations;
    const float3 step_uvw = uvw_dir * step_size;

    static const float3 albedo = float3(1.0f, 0.0f, 0.0f);

    if (uvw_len < step_size)
        return float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < iterations + 1; ++i) { 
        float val[8];
        val[0] = volume_tex.Sample(sampler2, cur_uvw + float3(-dr.x, -dr.y, -dr.z)).b;
        val[1] = volume_tex.Sample(sampler2, cur_uvw + float3( dr.x, -dr.y, -dr.z)).b;
        val[2] = volume_tex.Sample(sampler2, cur_uvw + float3(-dr.x, -dr.y,  dr.z)).b;
        val[3] = volume_tex.Sample(sampler2, cur_uvw + float3( dr.x, -dr.y,  dr.z)).b;
        val[4] = volume_tex.Sample(sampler2, cur_uvw + float3(-dr.x,  dr.y, -dr.z)).b;
        val[5] = volume_tex.Sample(sampler2, cur_uvw + float3( dr.x,  dr.y, -dr.z)).b;
        val[6] = volume_tex.Sample(sampler2, cur_uvw + float3(-dr.x,  dr.y,  dr.z)).b;
        val[7] = volume_tex.Sample(sampler2, cur_uvw + float3( dr.x,  dr.y,  dr.z)).b;

        float int_val[4];
        int_val[0] = lerp(val[0], val[1], 0.5f);
        int_val[1] = lerp(val[2], val[3], 0.5f);
        const float int_val_0 = lerp(int_val[0], int_val[1], 0.5f);
        int_val[2] = lerp(val[4], val[5], 0.5f);
        int_val[3] = lerp(val[6], val[7], 0.5f);
        const float int_val_1 = lerp(int_val[2], int_val[3], 0.5f);

        src = lerp(int_val_0, int_val_1, 0.5f);

        static const float absorption_coeff = 0.15f;
        if (src > 1e-3) {
            const float prev_visibility = dest_color.a;
            dest_color.a   *= exp(-(src * absorption_coeff) * step_size);
            const float absorption = prev_visibility - dest_color.a;
            dest_color.rgb += absorption * albedo * src;
        }

        if (dest_color.a <= 1e-4)
            break;

        cur_uvw += step_uvw;

        if (cur_uvw.x > 1.0f || cur_uvw.y > 1.0f || cur_uvw.z > 1.0f)
            break;
    }

    dest_color.a = 1.0f - dest_color.a;
    return dest_color;
}