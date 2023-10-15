RWTexture2D<float4> output;

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	output[DTid.xy] = float4(0.4353f, 0.3804f, 0.7529f, 1.0f);
}