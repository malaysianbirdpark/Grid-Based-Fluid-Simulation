struct PS_IN {
    float3 velocity    : VELOCITY;
};

float4 main(PS_IN input) : SV_TARGET
{
    return float4(input.velocity, 0.0f);
}