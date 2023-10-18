RWTexture2D<float> target : register(u0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    target[DTid.xy] = 0.0f;
}