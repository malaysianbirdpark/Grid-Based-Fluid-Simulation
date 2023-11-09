struct VS_IN {
    float3 pos      : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv       : TEXCOORD;
};

struct VS_OUT {
    float3 world_pos : POSITION;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float2 uv        : TEXCOORD;
    float4 sv_pos    : SV_POSITION;
};

cbuffer mvp : register(b0)
{
    matrix m;
    matrix mit;
    matrix mvp;
};

cbuffer mip : register(b1) {
    matrix mip;
}

VS_OUT main(VS_IN input)
{
	VS_OUT output;

    output.world_pos = mul(float4(input.pos, 1.0f), m).xyz;

    output.normal = normalize(mul(input.normal, (float3x3)mit).xyz);
    output.tangent = normalize(mul(input.tangent, (float3x3)m).xyz);

    output.uv = input.uv;
    output.sv_pos = mul(float4(input.pos, 1.0f), mvp);

    return output;
}