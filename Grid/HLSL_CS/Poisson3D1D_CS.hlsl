Texture3D<min16float> x_in : register(t0);
Texture3D<min16float> div : register(t1);

RWTexture3D<min16float> x_out : register(u0);

cbuffer constants : register(b2) {
	min16float alpha;
	min16float r_beta;
};

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	uint depth;
	x_in.GetDimensions(width, height, depth);

	//if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1 && DTid.z > 0 && DTid.z < depth - 1) 
	{
		const min16float center = x_in[DTid.xyz];

		const min16float right  = DTid.x + 1 != width - 1  ? x_in[uint3(DTid.x + 1, DTid.y, DTid.z)] : center;
		const min16float left   = DTid.x - 1 != 0          ? x_in[uint3(DTid.x - 1, DTid.y, DTid.z)] : center;
		const min16float up     = DTid.y + 1 != height - 1 ? x_in[uint3(DTid.x, DTid.y + 1, DTid.z)] : center;
		const min16float down   = DTid.y - 1 != 0          ? x_in[uint3(DTid.x, DTid.y - 1, DTid.z)] : center;
		const min16float front  = DTid.z + 1 != depth - 1  ? x_in[uint3(DTid.x, DTid.y, DTid.z + 1)] : center;
		const min16float behind = DTid.z - 1 != 0          ? x_in[uint3(DTid.x, DTid.y, DTid.z - 1)] : center;

		x_out[DTid.xyz] = (up + down + left + right + front + behind - div[DTid.xyz]) * 0.1666667f;
	}
}