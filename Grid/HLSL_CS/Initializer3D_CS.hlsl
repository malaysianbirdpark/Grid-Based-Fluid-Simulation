RWTexture3D<float> target : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    target[DTid.xyz] = 0.0f;
}