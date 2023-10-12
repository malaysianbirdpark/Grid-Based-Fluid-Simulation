struct VS_IN {
    float3 pos      : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct VS_OUT {
    float4 world_pos : POSITION;
    float2 uv        : TEXCOORD;
    float4 sv_pos    : SV_POSITION;
};

cbuffer mvp : register(b0)
{
    matrix m;
    matrix mit;
    matrix mvp;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output;

    output.world_pos = mul(float4(input.pos, 1.0f), m);
    output.uv = input.uv;
    output.sv_pos = mul(float4(input.pos, 1.0f), mvp);

    return output;
}