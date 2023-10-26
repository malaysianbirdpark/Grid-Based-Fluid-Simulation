struct PS_IN {
    float4 view_pos  : POSITION;
    float3 uvw       : TEXCOORD;
    float4 sv_pos    : SV_Position;
};

Texture2D previous : register(t0);

Texture3D volume_tex : register(t1);

Texture2D front_faces : register(t2);
Texture2D back_faces  : register(t3);

Texture2D front_texcoord : register(t4);
Texture2D back_texcoord  : register(t5);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);

cbuffer mvp : register(b0)
{
    matrix m;
    matrix mv;
    matrix mvp;
    float4 cam_pos;
};

//cbuffer Constants : register(b1) {
//    int step_size;
//    int iterations;
//};

float4 main(PS_IN input) : SV_Target
{
    //const float3 front = front_faces.Load(input.sv_pos);
    //const float3 back  = back_faces.Load(input.sv_pos);
    //const float  len   = length(back - front);
    //const float  len   = length(front - cam_pos.xyz);
    //if (len < 1e-2)
    //    return float4(0.0f, 0.0f, 0.0f, 1.0f);

    //const float3 ray_dir = back - front / len;
    //const float3 ray_dir = front - cam_pos / len;

    const float3 front_uvw = front_texcoord.Load(input.sv_pos);
    const float3 back_uvw  = back_texcoord.Load(input.sv_pos);
    const float  uvw_len   = length(back_uvw - front_uvw);
    const float3 uvw_dir   = back_uvw - front_uvw / uvw_len;
    
    //float3 cur_pos = front;
    float3 cur_uvw = front_uvw;

    float4 dest = 0.0f;
    float4 src = 0.0f;

    //float3 step = ray_dir * step_size;
    //float3 step = ray_dir * (len / 100.0f);
    float3 step_uvw = uvw_dir * (uvw_len / 100.0f);

    //for (int i = 0; i < iterations; ++i) {
    for (int i = 0; i < 102; ++i) { 
        src = volume_tex.Sample(sampler1, cur_uvw);

        dest.rgb += (1.0f - dest.a) * (src.rgb * src.a);
        dest.a += src.a * (1.0f - dest.a);

        if (dest.a >= 0.99f)
            break;

        //cur_pos += step;
        cur_uvw += step_uvw;

        if (cur_uvw.x > 1.0f || cur_uvw.y > 1.0f || cur_uvw.z > 1.0f)
            break;
    }

    return dest + previous.Load(input.sv_pos);
}