Texture3D<min16float> x_in     : register(t0);
Texture3D<min16float> residual : register(t1);

RWTexture3D<min16float> x_out : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	uint depth;
	x_in.GetDimensions(width, height, depth);

	const min16float center = x_in[DTid.xyz];
	const min16float right  = DTid.x < width - 1  ? x_in[uint3(DTid.x + 1, DTid.y, DTid.z)] : center;
	const min16float left   = DTid.x > 0          ? x_in[uint3(DTid.x - 1, DTid.y, DTid.z)] : center;
	const min16float up     = DTid.y < height - 1 ? x_in[uint3(DTid.x, DTid.y + 1, DTid.z)] : center;
	const min16float down   = DTid.y > 0          ? x_in[uint3(DTid.x, DTid.y - 1, DTid.z)] : center;
	const min16float front  = DTid.z < depth - 1  ? x_in[uint3(DTid.x, DTid.y, DTid.z + 1)] : center;
	const min16float behind = DTid.z > 0          ? x_in[uint3(DTid.x, DTid.y, DTid.z - 1)] : center;

	const min16float x = (up + down + left + right + front + behind - residual[DTid.xyz]) * 0.1111111f; // (1/6) * (2/3) = 1/9

	x_out[DTid.xyz] = x + center;
}