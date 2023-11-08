struct VS_IN {
    float3 pos      : POSITION;
};

struct VS_VEL {
    float3 prev     : PREV_POS;
    float3 velocity : VELOCITY;
};

struct VS_OUT {
    float4 pos       : SV_POSITION;
    float3 velocity  : VELOCITY;
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

VS_OUT main(VS_IN input, VS_VEL vel)
{
	VS_OUT output;

    output.pos = mul(mul(float4(input.pos, 1.0f), m), mip);
    output.velocity = vel.velocity;

    return output;
}