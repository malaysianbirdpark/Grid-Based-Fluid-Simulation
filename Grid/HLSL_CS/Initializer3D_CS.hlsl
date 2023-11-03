RWTexture3D<min16float> p1 : register(u0);
RWTexture3D<min16float> p2 : register(u1);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    p1[DTid.xyz] = 0.0f;
    p2[DTid.xyz] = 0.0f;
}