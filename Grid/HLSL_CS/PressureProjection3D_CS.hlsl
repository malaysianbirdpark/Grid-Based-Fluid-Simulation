Texture3D<min16float>  x_in       : register(t0);
Texture3D<min16float3> sub_target : register(t1);

RWTexture3D<min16float3> result     : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    x_in.GetDimensions(width, height, depth);

    if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1 && DTid.z > 0 && DTid.z < depth - 1) {
        const min16float right = x_in[uint3(DTid.x + 1, DTid.y, DTid.z)];
        const min16float left = x_in[uint3(DTid.x - 1, DTid.y, DTid.z)];
        const min16float up = x_in[uint3(DTid.x, DTid.y + 1, DTid.z)];
        const min16float down = x_in[uint3(DTid.x, DTid.y - 1, DTid.z)];
        const min16float front = x_in[uint3(DTid.x, DTid.y, DTid.z + 1)];
        const min16float behind = x_in[uint3(DTid.x, DTid.y, DTid.z - 1)];

        result[DTid.xyz].xyz = sub_target[DTid.xyz].xyz - min16float3(right - left, up - down, front - behind) * 0.5f;
    }
}