Texture3D<min16float3> x_in : register(t0);

Texture2DArray<uint>        obstacle : register(t6);
Texture2DArray<min16float3> obstacle_velocity : register(t7);

RWTexture3D<min16float> div : register(u0);

SamplerState sampler1 : register(s1);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	const min16float right = 
		obstacle[uint3(DTid.x + 1, DTid.y, DTid.z)].r <= 0.9f ? x_in[uint3(DTid.x + 1, DTid.y, DTid.z)].x : obstacle_velocity[uint3(DTid.x + 1, DTid.y, DTid.z)].x;
	const min16float left = 
		obstacle[uint3(DTid.x - 1, DTid.y, DTid.z)].r <= 0.9f ? x_in[uint3(DTid.x - 1, DTid.y, DTid.z)].x : obstacle_velocity[uint3(DTid.x - 1, DTid.y, DTid.z)].x;

	const min16float up = 
		obstacle[uint3(DTid.x, DTid.y + 1, DTid.z)].r <= 0.9f ? x_in[uint3(DTid.x, DTid.y + 1, DTid.z)].y : obstacle_velocity[uint3(DTid.x, DTid.y + 1, DTid.z)].y;
	const min16float down = 
		obstacle[uint3(DTid.x, DTid.y - 1, DTid.z)].r <= 0.9f ? x_in[uint3(DTid.x, DTid.y - 1, DTid.z)].y : obstacle_velocity[uint3(DTid.x, DTid.y - 1, DTid.z)].y;

	const min16float front = 
		obstacle[uint3(DTid.x, DTid.y, DTid.z + 1)].r <= 0.9f ? x_in[uint3(DTid.x, DTid.y, DTid.z + 1)].z : obstacle_velocity[uint3(DTid.x, DTid.y, DTid.z + 1)].z;
	const min16float behind = 
		obstacle[uint3(DTid.x, DTid.y, DTid.z - 1)].r <= 0.9f ? x_in[uint3(DTid.x, DTid.y, DTid.z - 1)].z : obstacle_velocity[uint3(DTid.x, DTid.y, DTid.z - 1)].z;

	div[DTid.xyz] = ((right - left) + (up - down) + (front - behind)) * 0.5f;
}