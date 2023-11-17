Texture3D<min16float> p : register(t0);

RWTexture3D<min16float> x : register(u0);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	x[DTid.xyz] += p[DTid.xyz];
}