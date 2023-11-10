struct VS_IN {
    float4 pos      : POSITION0;
    float4 prev     : POSITION1;
};

struct VS_OUT {
    float3 velocity  : VELOCITY;
    nointerpolation uint   slice : INDEX;
    float4 pos       : SV_POSITION;
};

cbuffer mvp : register(b0)
{
    matrix m;
    matrix mit;
    matrix mvp;
};

struct MIP {
    matrix data;
};

StructuredBuffer<MIP> mip : register(t0);

VS_OUT main(VS_IN input, uint id : SV_InstanceID)
{
	VS_OUT output;

    const float4 pos = mul(input.pos, m);
    output.pos = mul(pos, mip[id].data);
    output.velocity = (input.prev.xyz - pos.xyz) * 60.0f * 5.0f;
    output.slice = id;

    return output;
}