struct PS_IN {
    float4 view_pos   : POSITION;
    float3 uvw       : TEXCOORD;
    float4 sv_pos     : SV_POSITION;
};

float4 main(PS_IN input) : SV_TARGET
{
    return float4(input.view_pos);
}