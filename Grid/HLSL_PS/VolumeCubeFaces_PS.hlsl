struct PS_IN {
    float4 view_pos  : POSITION;
    float3 uvw       : TEXCOORD;
    float4 sv_pos    : SV_POSITION;
};

struct PS_OUT {
    float4 view_pos : SV_TARGET0;
    float3 uvw      : SV_TARGET1;
};

PS_OUT main(PS_IN input)
{
    PS_OUT output;

    output.view_pos = input.view_pos;
    output.uvw = input.uvw;

    return output;
}