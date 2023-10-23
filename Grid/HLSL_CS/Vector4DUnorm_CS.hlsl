Texture2D<float4> input : register(t0);

RWTexture2D<float4> output : register(u0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	const float4 color = input[DTid.xy] / 65504.0f;
	output[DTid.xy] = color;
}