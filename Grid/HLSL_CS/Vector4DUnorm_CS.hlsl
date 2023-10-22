Texture2D<float4> input : register(t0);

RWTexture2D<float4> output : register(u0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    output[DTid.xy] = (normalize(input[DTid.xy]) + 1.0f) * 127.5f;
}