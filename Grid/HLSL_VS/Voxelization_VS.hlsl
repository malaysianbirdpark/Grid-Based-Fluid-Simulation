struct VS_IN {
    float3 pos      : POSITION;
};

struct VS_OUT {
    float4 sv_pos    : SV_POSITION;
};

cbuffer mvp : register(b0)
{
    matrix m;
    matrix mit;
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