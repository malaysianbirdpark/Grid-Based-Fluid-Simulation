RWTexture2D<float> x_in : register(u0);

RWTexture2D<float2> sub_target : register(u1);
RWTexture2D<float2> result : register(u2);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    x_in.GetDimensions(width, height);

    const float up = x_in[uint2(DTid.x, min(DTid.y + 1, height - 1))];
    const float down = x_in[uint2(DTid.x, max(DTid.y - 1, 0))];
    const float right = x_in[uint2(min(DTid.x + 1, width - 1), DTid.y)];
    const float left = x_in[uint2(max(DTid.x - 1, 0), DTid.y)];

	result[DTid.xy].xy = sub_target[DTid.xy].xy - float2(right - left, up - down) * 0.5f;
}