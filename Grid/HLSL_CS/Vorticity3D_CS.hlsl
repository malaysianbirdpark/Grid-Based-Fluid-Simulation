Texture3D<min16float3> velocity : register(t0);

RWTexture3D<min16float3> vorticity : register(u0);

cbuffer constants : register(b1) {
	min16float dt;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    velocity.GetDimensions(width, height, depth);

	const uint3 right  = DTid.x + 1 < width - 1  ? uint3(DTid.x + 1, DTid.yz) : DTid.xyz;
	const uint3 left   = DTid.x - 1 > 0          ? uint3(DTid.x - 1, DTid.yz) : DTid.xyz;

	const uint3 up     = DTid.y + 1 < height - 1 ? uint3(DTid.x, DTid.y + 1, DTid.z) : DTid.xyz;
	const uint3 down   = DTid.y - 1 > 0          ? uint3(DTid.x, DTid.y - 1, DTid.z) : DTid.xyz;

	const uint3 front  = DTid.z + 1 < depth - 1  ? uint3(DTid.xy, DTid.z + 1) : DTid.xyz;
	const uint3 behind = DTid.z - 1 > 0          ? uint3(DTid.xy, DTid.z - 1) : DTid.xyz;

	const min16float Fz_y = (velocity[up].z - velocity[down].z) * 0.5f;
	const min16float Fy_z = (velocity[front].y - velocity[behind].y) * 0.5f;

	const min16float Fx_z = (velocity[front].x - velocity[behind].x) * 0.5f;
	const min16float Fz_x = (velocity[right].z - velocity[left].z) * 0.5f;

	const min16float Fy_x = (velocity[right].y - velocity[left].y) * 0.5f;
	const min16float Fx_y = (velocity[up].x - velocity[down].x) * 0.5f;

	vorticity[DTid.xyz] = min16float3(Fz_y - Fy_z, Fx_z - Fz_x, Fy_x - Fx_y);
}