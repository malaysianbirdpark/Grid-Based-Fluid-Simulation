struct PS_IN {
    float4 view_pos  : POSITION;
    float3 uvw       : TEXCOORD;
    float4 sv_pos    : SV_Position;
};

Texture2D previous : register(t0);

Texture3D volume_tex : register(t1);

Texture2D front_texcoord : register(t2);
Texture2D back_texcoord  : register(t3);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);

//cbuffer Constants : register(b1) {
//    int step_size;
//    int iterations;
//};

float4 main(PS_IN input) : SV_Target
{
    const float3 front_uvw = front_texcoord.Load(input.sv_pos);
    const float3 back_uvw  = back_texcoord.Load(input.sv_pos);
    const float  uvw_len   = length(back_uvw - front_uvw);
    if (uvw_len < 0.01)
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    const float3 uvw_dir   = (back_uvw - front_uvw) / uvw_len;
    
    float3 cur_uvw = front_uvw;

    float4 dest_color = 0.0f;
    float4 src_color = 0.0f;

    const float step_size = 0.01f;
    float3 step_uvw = uvw_dir * step_size;
    const float hs = step_size * 1.0f;

    const int iterations = (uvw_len / step_size) + 1;

    [loop]
    for (int i = 0; i < iterations; ++i) { 
        //const float3 a = cur_uvw + float3(-hs, -hs, -hs);
        //const float3 b = cur_uvw + float3(-hs, -hs,  hs);
        //const float4 m1 = (volume_tex.SampleLevel(sampler2, a, 0.0f) + volume_tex.SampleLevel(sampler2, b, 0.0f)) * 0.5f;

        //const float3 c = cur_uvw + float3(-hs,  hs, -hs);
        //const float3 d = cur_uvw + float3(-hs,  hs,  hs);
        //const float4 m2 = (volume_tex.SampleLevel(sampler2, c, 0.0f) + volume_tex.SampleLevel(sampler2, d, 0.0f)) * 0.5f;

        //const float4 m3 = (m1 + m2) * 0.5f;

        //const float3 e = cur_uvw + float3( hs, -hs, -hs);
        //const float3 f = cur_uvw + float3( hs, -hs,  hs);
        //const float4 n1 = (volume_tex.SampleLevel(sampler2, e, 0.0f) + volume_tex.SampleLevel(sampler2, f, 0.0f)) * 0.5f;

        //const float3 g = cur_uvw + float3( hs,  hs, -hs);
        //const float3 h = cur_uvw + float3( hs,  hs,  hs);
        //const float4 n2 = (volume_tex.SampleLevel(sampler2, g, 0.0f) + volume_tex.SampleLevel(sampler2, h, 0.0f)) * 0.5f;

        //const float4 n3 = (n1 + n2) * 0.5f;

        //src_color = (m3 + n3) * 0.5f;

        src_color = (volume_tex.Sample(sampler0, cur_uvw) + volume_tex.Sample(sampler0, cur_uvw + (step_uvw * 0.5f))) * 0.5f;

        dest_color.rgb += (1.0f - dest_color.a) * src_color.rgb * src_color.a;
        dest_color.a   += (1.0f - dest_color.a) * src_color.a;

        if (dest_color.a >= 0.99f)
            break;

        cur_uvw += step_uvw;

        if (cur_uvw.x > 1.0f || cur_uvw.y > 1.0f || cur_uvw.z > 1.0f) {
			dest_color.rgb += (1.0f - dest_color.a) * src_color.rgb * src_color.a * ((step_size - length(cur_uvw - back_uvw)) / step_size);
			dest_color.a   += (1.0f - dest_color.a) * src_color.a;

            break;
        }
    }

    //return dest_color + previous.Load(input.sv_pos);
    //return float4(dest_color.rgb, 1.0f);
    return dest_color;
}