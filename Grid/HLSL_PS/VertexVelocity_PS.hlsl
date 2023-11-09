struct ps_in {
    float3 velocity    : VELOCITY;
};

float4 main(ps_in input) : sv_target
{
    //return float4(input.velocity, 0.0f);
    return float4(10.0f, 10.0f,100.0f, 0.0f);
}