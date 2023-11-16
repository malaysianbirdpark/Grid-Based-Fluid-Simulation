RWTexture3D<min16float> p0  : register(u0);
RWTexture3D<min16float> p1  : register(u1);
RWTexture3D<min16float> p2  : register(u2);
RWTexture3D<min16float> p3  : register(u3);
RWTexture3D<min16float> p4  : register(u4);
RWTexture3D<min16float> p5  : register(u5);
RWTexture3D<min16float> p6  : register(u6);
RWTexture3D<min16float> p7  : register(u7);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    p0[DTid.xyz] = 0.0f;
    p1[DTid.xyz] = 0.0f;
    p2[DTid.xyz] = 0.0f;
    p3[DTid.xyz] = 0.0f;
    p4[DTid.xyz] = 0.0f;
    p5[DTid.xyz] = 0.0f;
    p6[DTid.xyz] = 0.0f;
    p7[DTid.xyz] = 0.0f;
}