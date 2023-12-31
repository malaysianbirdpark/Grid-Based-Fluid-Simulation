struct VS_IN {
    float3 pos : POSITION;
    float3 uvw : TEXCOORD;
};

struct VS_OUT {
    float3 world_pos : POSITION;
    float3 uvw       : TEXCOORD;
    float4 sv_pos    : SV_Position;
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
    output.uvw       = input.uvw;
    output.sv_pos    = mul(float4(input.pos, 1.0f), mvp);

    return output;
}