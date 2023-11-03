Texture3D<min16float3> vorticity : register(t0);

RWTexture3D<min16float3> velocity : register(u0);

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

	if (DTid.x > 1 && DTid.y > 1 && DTid.z > 1 && DTid.x < width - 2 && DTid.y < height - 2 && DTid.z < depth - 2) {
        min16float right  = length(vorticity[uint3(DTid.x + 1, DTid.y, DTid.z)]);
        min16float left   = length(vorticity[uint3(DTid.x - 1, DTid.y, DTid.z)]);
        min16float up     = length(vorticity[uint3(DTid.x, DTid.y + 1, DTid.z)]);
        min16float down   = length(vorticity[uint3(DTid.x, DTid.y - 1, DTid.z)]);
        min16float front  = length(vorticity[uint3(DTid.x, DTid.y, DTid.z + 1)]);
        min16float behind = length(vorticity[uint3(DTid.x, DTid.y, DTid.z - 1)]);

        const min16float3 t = min16float3(right - left, up - down, front - behind);
		
		const min16float3 n = normalize(t);
		const min16float3 v = velocity[DTid.xyz];
		const min16float3 f = 1e-1 * cross(n, v);

		velocity[DTid.xyz] += f * dt;
	}
}