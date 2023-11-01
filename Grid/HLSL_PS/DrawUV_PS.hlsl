struct PS_IN {
    float4 view_pos  : POSITION;
    float3 uvw       : TEXCOORD;
    float4 sv_pos    : SV_POSITION;
};

float3 main(PS_IN input) : SV_TARGET
{
    return input.uvw;
}