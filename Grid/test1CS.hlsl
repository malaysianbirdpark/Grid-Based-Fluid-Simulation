RWTexture2D<float4> output;

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	if (DTid.x >= 400)
		output[DTid.xy] = float4(0.7353f, 0.1804f, 0.3529f, 1.0f);
}