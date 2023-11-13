Texture3D<min16float3> vorticity : register(t0);

Texture2DArray<uint> obstacle : register(t6);

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

    //if (obstacle[DTid.xyz].r <= 0.9f)
    {
        const min16float right = length(vorticity[uint3(DTid.x + 1, DTid.y, DTid.z)]);
        const min16float left = length(vorticity[uint3(DTid.x - 1, DTid.y, DTid.z)]);
        const min16float up = length(vorticity[uint3(DTid.x, DTid.y + 1, DTid.z)]);
        const min16float down = length(vorticity[uint3(DTid.x, DTid.y - 1, DTid.z)]);
        const min16float front = length(vorticity[uint3(DTid.x, DTid.y, DTid.z + 1)]);
        const min16float behind = length(vorticity[uint3(DTid.x, DTid.y, DTid.z - 1)]);

        const min16float3 t = min16float3(right - left, up - down, front - behind) * 0.5f;

        if (length(t) < 1e-4)
            return;
		
        const min16float3 n = normalize(t);
        const min16float3 w = vorticity[DTid.xyz];
        const min16float3 f = 6.5f * cross(n, w);

        velocity[DTid.xyz] += f * dt;
    }
}