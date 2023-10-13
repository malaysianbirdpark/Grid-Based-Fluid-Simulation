RWTexture2D<float4> output;

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	if (DTid.x >= 600)
		output[DTid.xy] = float4(0.1353f, 0.1804f, 0.6529f, 1.0f);
}