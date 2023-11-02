struct PS_IN {
    float4 view_pos  : POSITION;
    float3 uvw       : TEXCOORD;
    float4 sv_pos    : SV_Position;
};

Texture3D<min16float4> volume_tex : register(t0);

Texture2D front_texcoord : register(t1);
Texture2D back_texcoord  : register(t2);

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
    float src = 0.0f;

    //const float step_size = 0.01f;
    //float3 step_uvw = uvw_dir * step_size;

    //const int iterations = (uvw_len / step_size) + 1;

    static const int iterations = 50;
    static const float att = 0.15f;

    const float step_size = uvw_len / iterations;
    float3 step_uvw = uvw_dir * step_size;

    [loop]
    for (int i = 0; i < iterations + 1; ++i) { 
        src = (volume_tex.Sample(sampler2, cur_uvw).a + volume_tex.Sample(sampler2, cur_uvw + (step_uvw * 0.5f)).a) * 0.5f;
 
        if (src > 1e-2) {
			dest_color.rgb += (1.0f - dest_color.a) * float3(0.0f, 1.0f, 0.0f) * att;
			dest_color.a   += (1.0f - dest_color.a) * att;
        }

        if (dest_color.a >= 0.99f)
            break;

        cur_uvw += step_uvw;

        if (cur_uvw.x > 1.0f || cur_uvw.y > 1.0f || cur_uvw.z > 1.0f) {
			//dest_color.rgb += (1.0f - dest_color.a) * src_color.rgb * src_color.a * ((step_size - length(cur_uvw - back_uvw)) / step_size);
			//dest_color.a   += (1.0f - dest_color.a) * src_color.a;

            break;
        }
    }

    //return float4(dest_color.rgb, 1.0f);
    return dest_color;
}