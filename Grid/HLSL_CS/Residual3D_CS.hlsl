Texture3D<min16float> p_in : register(t0);
Texture3D<min16float> x_in : register(t1);

RWTexture3D<min16float> x_out : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	x_out[DTid.xyz] = x_in[DTid.xyz] - p_in[DTid.xyz];
}