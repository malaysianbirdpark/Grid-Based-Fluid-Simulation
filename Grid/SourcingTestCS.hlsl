RWTexture2D<float2> velocity : register(u0);
RWTexture2D<float4> density : register(u1);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    density.GetDimensions(width, height);

    density[DTid.xy] = float4(0.25f, 0.15f, 0.35f, 1.0f);
    velocity[DTid.xy] = float2(0.15f, 0.24f);
}