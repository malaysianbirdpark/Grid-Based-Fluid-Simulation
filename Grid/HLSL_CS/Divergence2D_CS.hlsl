RWTexture2D<float2> x_in : register(u1);

RWTexture2D<float> div : register(u0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    x_in.GetDimensions(width, height);

    const float2 up    = x_in[uint2(DTid.x, min(DTid.y + 1, height - 1))].xy;
    const float2 down  = x_in[uint2(DTid.x, max(DTid.y - 1, 0))].xy;
    const float2 right = x_in[uint2(min(DTid.x + 1, width - 1), DTid.y)].xy;
    const float2 left  = x_in[uint2(max(DTid.x - 1, 0), DTid.y)].xy;

    div[DTid.xy] = ((right.x - left.x) + (up.y - down.y)) * 0.5f;
}