struct VS_IN {
    float3 pos      : POSITION;
};

struct VS_OUT {
    float4 view_pos  : POSITION;
    float4 sv_pos    : SV_POSITION;
};

cbuffer mvp : register(b0)
{
    matrix m;
    matrix mv;
    matrix mvi;
    matrix mvp;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output;

    output.view_pos  = mul(float4(input.pos, 1.0f), mv);
    output.sv_pos    = mul(float4(input.pos, 1.0f), mvp);

    return output;
}