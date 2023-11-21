Texture3D<min16float> p : register(t0);
Texture3D<min16float> x : register(t1);

RWTexture3D<min16float> x_out : register(u0);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint width;
	uint height;
	uint depth;
	p.GetDimensions(width, height, depth);

	const min16float center = p[DTid.xyz];
	const min16float right  = DTid.x < width - 1  ? p[uint3(DTid.x + 1, DTid.y, DTid.z)] : center;
	const min16float left   = DTid.x > 0          ? p[uint3(DTid.x - 1, DTid.y, DTid.z)] : center;
	const min16float up     = DTid.y < height - 1 ? p[uint3(DTid.x, DTid.y + 1, DTid.z)] : center;
	const min16float down   = DTid.y > 0          ? p[uint3(DTid.x, DTid.y - 1, DTid.z)] : center;
	const min16float front  = DTid.z < depth - 1  ? p[uint3(DTid.x, DTid.y, DTid.z + 1)] : center;
	const min16float behind = DTid.z > 0          ? p[uint3(DTid.x, DTid.y, DTid.z - 1)] : center;
	
	const min16float lhs = (right + left + up + down + front + behind) - 6.0f * center;

	x_out[DTid.xyz] = x[DTid.xyz] - lhs;
}