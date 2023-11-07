Texture3D<min16float3> x_in : register(t0);

Texture2DArray obstacle : register(t6);

RWTexture3D<min16float> div : register(u0);

SamplerState sampler1 : register(s1);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    x_in.GetDimensions(width, height, depth);

	const min16float right = 
		obstacle[uint3(DTid.x + 1, DTid.y, DTid.z)].w <= 0.9f ? x_in[uint3(DTid.x + 1, DTid.y, DTid.z)].x : 0.0f;
	const min16float left = 
		obstacle[uint3(DTid.x - 1, DTid.y, DTid.z)].w <= 0.9f ? x_in[uint3(DTid.x - 1, DTid.y, DTid.z)].x : 0.0f;

	const min16float up = 
		obstacle[uint3(DTid.x, DTid.y + 1, DTid.z)].w <= 0.9f ? x_in[uint3(DTid.x, DTid.y + 1, DTid.z)].y : 0.0f;
	const min16float down = 
		obstacle[uint3(DTid.x, DTid.y - 1, DTid.z)].w <= 0.9f ? x_in[uint3(DTid.x, DTid.y - 1, DTid.z)].y : 0.0f;

	const min16float front = 
		obstacle[uint3(DTid.x, DTid.y, DTid.z + 1)].w <= 0.9f ? x_in[uint3(DTid.x, DTid.y, DTid.z + 1)].z : 0.0f;
	const min16float behind = 
		obstacle[uint3(DTid.x, DTid.y, DTid.z - 1)].w <= 0.9f ? x_in[uint3(DTid.x, DTid.y, DTid.z - 1)].z : 0.0f;

	div[DTid.xyz] = ((right - left) + (up - down) + (front - behind)) * 0.5f;
}