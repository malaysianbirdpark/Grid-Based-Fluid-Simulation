Texture3D<min16float3> x_in : register(t0);

RWTexture3D<min16float> div : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    x_in.GetDimensions(width, height, depth);

	const min16float right  = DTid.x + 1 != width - 1  ? x_in[uint3(DTid.x + 1, DTid.y, DTid.z)].x : 0.0f;
	const min16float left   = DTid.x - 1 != 0          ? x_in[uint3(DTid.x - 1, DTid.y, DTid.z)].x : 0.0f;

	const min16float up     = DTid.y + 1 != height - 1 ? x_in[uint3(DTid.x, DTid.y + 1, DTid.z)].y : -1.0f;
	const min16float down   = DTid.y - 1 != 0          ? x_in[uint3(DTid.x, DTid.y - 1, DTid.z)].y : 1.0f;

	const min16float front  = DTid.z + 1 != depth - 1  ? x_in[uint3(DTid.x, DTid.y, DTid.z + 1)].z : 0.0f;
	const min16float behind = DTid.z - 1 != 0          ? x_in[uint3(DTid.x, DTid.y, DTid.z - 1)].z : 0.0f;

	div[DTid.xyz] = ((right - left) + (up - down) + (front - behind)) * 0.5f;
}