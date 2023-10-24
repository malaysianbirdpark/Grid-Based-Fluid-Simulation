RWTexture3D<float3> x_in : register(u1);

RWTexture3D<float> div : register(u0);

[numthreads(16, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    x_in.GetDimensions(width, height, depth);

    const float3 top    = x_in[uint3(DTid.x, min(DTid.y + 1, height - 1), DTid.z)].xyz;
    const float3 bottom  = x_in[uint3(DTid.x, max(DTid.y - 1, 0), DTid.z)].xyz;
    const float3 right = x_in[uint3(min(DTid.x + 1, width - 1), DTid.y, DTid.z)].xyz;
    const float3 left  = x_in[uint3(max(DTid.x - 1, 0), DTid.y, DTid.z)].xyz;
    const float3 front  = x_in[uint3(DTid.x, DTid.y, min(DTid.z + 1, depth - 1)].xyz;
    const float3 behind  = x_in[uint3(DTid.x, DTid.y, max(DTid.z - 1, 0)].xyz;

    div[DTid.xy] = ((right.x - left.x) + (up.y - down.y) + (front.z - behind.z)) * 0.5f;
}