Texture3D<min16float> x_in : register(t0);
Texture3D<min16float> div : register(t1);

Texture2DArray<uint> obstacle : register(t6);

RWTexture3D<min16float> x_out : register(u0);

SamplerState sampler1 : register(s1);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	{
		const min16float4 center = x_in[DTid.xyz]; 

		const min16float right = 
			obstacle[uint3(DTid.x + 1, DTid.y, DTid.z)].r <= 0.9f ? x_in[uint3(DTid.x + 1, DTid.y, DTid.z)] : center;
		const min16float left = 
			obstacle[uint3(DTid.x - 1, DTid.y, DTid.z)].r <= 0.9f ? x_in[uint3(DTid.x - 1, DTid.y, DTid.z)] : center;
		const min16float up = 
			obstacle[uint3(DTid.x, DTid.y + 1, DTid.z)].r <= 0.9f ? x_in[uint3(DTid.x, DTid.y + 1, DTid.z)] : center;
		const min16float down = 
			obstacle[uint3(DTid.x, DTid.y - 1, DTid.z)].r <= 0.9f ? x_in[uint3(DTid.x, DTid.y - 1, DTid.z)] : center;
		const min16float front = 
			obstacle[uint3(DTid.x, DTid.y, DTid.z + 1)].r <= 0.9f ? x_in[uint3(DTid.x, DTid.y, DTid.z + 1)] : center;
		const min16float behind = 
			obstacle[uint3(DTid.x, DTid.y, DTid.z - 1)].r <= 0.9f ? x_in[uint3(DTid.x, DTid.y, DTid.z - 1)] : center;

		x_out[DTid.xyz] = (up + down + left + right + front + behind - div[DTid.xyz]) * 0.1666667f;
	}
}