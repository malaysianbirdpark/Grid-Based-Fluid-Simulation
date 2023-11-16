Texture3D<min16float> x_in     : register(t0);
Texture3D<min16float> residual : register(t1);

RWTexture3D<min16float> x_out : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	const min16float right  = x_in[uint3(DTid.x + 1, DTid.y, DTid.z)];
	const min16float left   = x_in[uint3(DTid.x - 1, DTid.y, DTid.z)];
	const min16float up     = x_in[uint3(DTid.x, DTid.y + 1, DTid.z)];
	const min16float down   = x_in[uint3(DTid.x, DTid.y - 1, DTid.z)];
	const min16float front  = x_in[uint3(DTid.x, DTid.y, DTid.z + 1)];
	const min16float behind = x_in[uint3(DTid.x, DTid.y, DTid.z - 1)];

	x_out[DTid.xyz] = (up + down + left + right + front + behind - residual[DTid.xyz]) * 0.1666667f;
}