struct PS_IN {
    float4 world_pos  : POSITION;
    float4 sv_pos    : SV_POSITION;
};

float4 main(PS_IN input) : SV_TARGET
{
    return float4(input.world_pos);
}