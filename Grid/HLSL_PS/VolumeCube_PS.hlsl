struct PS_IN {
    float4 world_pos : POSITION;
    float3 uvw       : TEXCOORD;
};

float4 main(PS_IN input) : SV_Target
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}