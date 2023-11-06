struct VS_IN {
    float3 pos      : POSITION;
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float3 binormal : BINORMAL;
    float2 uv       : TEXCOORD;
};

struct VS_OUT {
    float4 world_pos : POSITION;
    float4 sv_pos    : SV_POSITION;
};

cbuffer mvp : register(b0)
{
    matrix m;
    matrix mit;
    matrix mv;
    matrix mp;
    matrix mvp;
};

cbuffer mip : register(b1)
{
    matrix mip;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output;

    output.sv_pos = mul(mul(float4(input.pos, 1.0f), m), mip);

    return output;
}