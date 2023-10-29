RWTexture3D<min16float> value : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    value.GetDimensions(width, height, depth);

    if (DTid.x == 0) {
        value[DTid.xyz] = value[uint3(DTid.x + 1, DTid.y, DTid.z)];
    }
    else if (DTid.y == 0) {
        value[DTid.xyz] = value[uint3(DTid.x, DTid.y + 1, DTid.z)];
    }
    else if (DTid.z == 0) {
        value[DTid.xyz] = value[uint3(DTid.x, DTid.y, DTid.z + 1)];
    }
    else if (DTid.x == width - 1) {
        value[DTid.xyz] = value[uint3(DTid.x - 1, DTid.y, DTid.z)];
    }
    else if (DTid.y == height - 1) {
        value[DTid.xyz] = value[uint3(DTid.x, DTid.y - 1, DTid.z)];
    }
    else if (DTid.z == depth - 1) {
        value[DTid.xyz] = value[uint3(DTid.x, DTid.y, DTid.z - 1)];
    }
}