RWTexture3D<float>  x_in       : register(u0);

RWTexture3D<float3> sub_target : register(u1);
RWTexture3D<float3> result     : register(u2);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    x_in.GetDimensions(width, height, depth);

    const float up = x_in[uint3(DTid.x, min(DTid.y + 1, height - 1), DTid.z)];
    const float down = x_in[uint3(DTid.x, max(DTid.y - 1, 0), DTid.z)];
    const float right = x_in[uint3(min(DTid.x + 1, width - 1), DTid.y, DTid.z)];
    const float left = x_in[uint3(max(DTid.x - 1, 0), DTid.y, DTid.z)];
    const float front = x_in[uint3(DTid.x, DTid.y, min(DTid.z + 1, depth - 1))];
    const float behind = x_in[uint3(DTid.x, DTid.y, max(DTid.z - 1, 0))];

	result[DTid.xyz].xyz = sub_target[DTid.xyz].xyz - float3(right - left, up - down, front - behind) * 0.5f;
}