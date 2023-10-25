struct PS_IN {
    float4 view_pos   : POSITION;
    float4 sv_pos     : SV_Position;
};

Texture2D previous : register(t0);

Texture3D volume_tex : register(t1);

Texture2D front_faces : register(t2);
Texture2D back_faces  : register(t3);

SamplerState sampler0 : register(s0);

cbuffer Transform : register(b0) {
    matrix m;
    matrix mv;
    matrix mvi;
    matrix mvp;
}

cbuffer Constants : register(b1) {
    int step_size;
    int iterations;
};

float4 main(PS_IN input) : SV_Target
{
    float2 uv = input.sv_pos.xy /= input.sv_pos.w;
    uv.x = 0.5f * uv.x + 0.5f;
    uv.y = -0.5f * uv.y + 0.5f;

    const float3 front = front_faces.Sample(sampler0, uv).xyz;
    const float3 back = back_faces.Sample(sampler0, uv).xyz;
    const float3 ray_dir = normalize(back - front);
    
    float3 cur_pos = front;

    float4 dest = 0.0f;
    float4 src = 0.0f;

    //float3 step = ray_dir * step_size;
    float3 step = ray_dir * 0.02f;

    //for (int i = 0; i < iterations; ++i) {
    for (int i = 0; i < 55; ++i) {
        float3 uvw = mul(float4(cur_pos, 1.0f), mvi);
		uvw.x = 0.5f * uvw.x + 0.5f;
		uvw.y = -0.5f * uvw.y + 0.5f;
		uvw.z = 0.5f * uvw.z + 0.5f;
        src = volume_tex.Sample(sampler0, uvw);
        //src = float4(0.5f, 0.22f, 0.39f, 0.2f);

        src.rgb *= src.a;
        dest = (1.0f - dest.a) * src + dest;

        if (dest.a >= 0.99f)
            break;

        cur_pos += step;

        if (uvw.x > 1.0f || uvw.y > 1.0f || uvw.z > 1.0f)
            break;
    }

    return dest + previous.Load(input.sv_pos);
}