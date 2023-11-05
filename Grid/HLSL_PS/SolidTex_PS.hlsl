struct PS_IN {
    float4 view_pos  : POSITION;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 binormal  : BINORMAL;
    float2 uv        : TEXCOORD;
    float4 sv_pos    : SV_POSITION;
};

struct PS_OUT {
    float4 color : SV_TARGET;
};

Texture2D albedo : register(t0);

SamplerState sampler2 : register(s2);

PS_OUT main(PS_IN input)
{
    PS_OUT output;

    const float alpha = 1.0f - albedo.Sample(sampler2, input.uv).r;
    output.color = float4(1.0f, 1.0f, 1.0f, alpha);

    return output;
}