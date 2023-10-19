RWTexture2D<float2> x_in : register(u1);

RWTexture2D<float> div : register(u0);

SamplerState sampler0 : register(s0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    x_in.GetDimensions(width, height);

    const float2 up = x_in[uint2(DTid.x, min(DTid.y + 1, height))];
    const float2 down = x_in[uint2(DTid.x, max(DTid.y - 1, 0))];
    const float2 right = x_in[uint2(min(DTid.x + 1, width), DTid.y)];
    const float2 left = x_in[uint2(max(DTid.x - 1, 0), DTid.y)];

    div[DTid.xy] = ((right.x - left.x) + (up.y - down.y)) * 0.5f;
}