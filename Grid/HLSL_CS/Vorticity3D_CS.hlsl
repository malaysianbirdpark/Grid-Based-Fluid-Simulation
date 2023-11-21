Texture3D<min16float4> velocity : register(t0);

Texture2DArray<uint> obstacle : register(t10);

RWTexture3D<min16float3> vorticity : register(u0);

cbuffer constants : register(b1) {
	min16float dt;
}

cbuffer Dimension : register(b3) 
{
    uint width;
    uint height;
    uint depth;
    float reciprocal_width;
    float reciprocal_height;
    float reciprocal_depth;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	//if (obstacle[DTid.xyz].r <= 0.9f) 
	{
		const uint3 right  = DTid.x + 1 < width - 1  ? uint3(DTid.x + 1, DTid.yz) : DTid.xyz;
		const uint3 left   = DTid.x - 1 > 0          ? uint3(DTid.x - 1, DTid.yz) : DTid.xyz;

		const uint3 up     = DTid.y + 1 < height - 1 ? uint3(DTid.x, DTid.y + 1, DTid.z) : DTid.xyz;
		const uint3 down   = DTid.y - 1 > 0          ? uint3(DTid.x, DTid.y - 1, DTid.z) : DTid.xyz;

		const uint3 front  = DTid.z + 1 < depth - 1  ? uint3(DTid.xy, DTid.z + 1) : DTid.xyz;
		const uint3 behind = DTid.z - 1 > 0          ? uint3(DTid.xy, DTid.z - 1) : DTid.xyz;

		const min16float Fz_y = (velocity[up].z  - velocity[down].z);
		const min16float Fy_z = (velocity[front].y - velocity[behind].y);

		const min16float Fx_z = (velocity[front].x - velocity[behind].x);
		const min16float Fz_x = (velocity[right].z - velocity[left].z);

		const min16float Fy_x = (velocity[right].y - velocity[left].y);
		const min16float Fx_y = (velocity[up].x - velocity[down].x);

		vorticity[DTid.xyz] = min16float3(Fz_y - Fy_z, Fx_z - Fz_x, Fy_x - Fx_y) * 0.5f;
	}
}