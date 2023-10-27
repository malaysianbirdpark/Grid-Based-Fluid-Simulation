RWTexture3D<min16float3> x_in : register(u1);

RWTexture3D<min16float> div : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    x_in.GetDimensions(width, height, depth);

    const min16float right = x_in[uint3(min(DTid.x + 1, width - 1), DTid.y, DTid.z)].x;
    const min16float left  = x_in[uint3(max(DTid.x - 1, 0),         DTid.y, DTid.z)].x;

    const min16float up    = x_in[uint3(DTid.x, min(DTid.y + 1, height - 1), DTid.z)].y;
    const min16float down  = x_in[uint3(DTid.x, max(DTid.y - 1, 0),          DTid.z)].y;

    const min16float front   = x_in[uint3(DTid.x, DTid.y, min(DTid.z + 1, depth - 1))].z;
    const min16float behind  = x_in[uint3(DTid.x, DTid.y, max(DTid.z - 1, 0))].z;

    div[DTid.xyz] = ((right - left) + (up - down) + (front - behind)) * 0.5f;
}