Texture3D<min16float> p : register(t0);
Texture3D<min16float> x : register(t1);

RWTexture3D<min16float> x_out : register(u0);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	const min16float center = p[DTid.xyz]; 
	const min16float right  = p[uint3(DTid.x + 1, DTid.y, DTid.z)];
	const min16float left   = p[uint3(DTid.x - 1, DTid.y, DTid.z)];
	const min16float up     = p[uint3(DTid.x, DTid.y + 1, DTid.z)];
	const min16float down   = p[uint3(DTid.x, DTid.y - 1, DTid.z)];
	const min16float front  = p[uint3(DTid.x, DTid.y, DTid.z + 1)];
	const min16float behind = p[uint3(DTid.x, DTid.y, DTid.z - 1)];
	
	const min16float lhs = (right + left + up + down + front + behind) - 6.0f * center;

	x_out[DTid.xyz] = x[DTid.xyz] - lhs;
}