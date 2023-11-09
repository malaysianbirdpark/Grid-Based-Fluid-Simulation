struct VS_IN {
    float4 pos      : POSITION0;
    float4 prev     : POSITION1;
};

struct VS_OUT {
    float3 velocity  : VELOCITY;
    float4 pos       : SV_POSITION;
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

cbuffer timestep : register(b2)
{
    float dt;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output;

    const float4 pos = mul(input.pos, m);
    output.pos = mul(pos, mip);
    output.velocity = (input.prev.xyz - pos.xyz) * 60.0f;

    return output;
}