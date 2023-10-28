RWTexture3D<min16float>  x_in       : register(u0);

RWTexture3D<min16float3> sub_target : register(u1);
RWTexture3D<min16float3> result     : register(u2);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    x_in.GetDimensions(width, height, depth);

    //if (DTid.x == 0 || DTid.x == width - 1 || DTid.y == 0 || DTid.y == height - 1 || DTid.z == 0 || DTid.z == depth - 1) {
    //    result[DTid.xyz] = 0.0f;
    //    return;
    //}

    const min16float right  = x_in[uint3(min(DTid.x + 1, width - 1), DTid.y, DTid.z)];
    const min16float left   = x_in[uint3(max(DTid.x - 1, 0), DTid.y, DTid.z)];
    const min16float up     = x_in[uint3(DTid.x, min(DTid.y + 1, height - 1), DTid.z)];
    const min16float down   = x_in[uint3(DTid.x, max(DTid.y - 1, 0), DTid.z)];
    const min16float front  = x_in[uint3(DTid.x, DTid.y, min(DTid.z + 1, depth - 1))];
    const min16float behind = x_in[uint3(DTid.x, DTid.y, max(DTid.z - 1, 0))];

	result[DTid.xyz].xyz = sub_target[DTid.xyz].xyz - min16float3(right - left, up - down, front - behind) * 0.5f;
}