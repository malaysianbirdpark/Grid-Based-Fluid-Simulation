RWTexture2D<float4> output;

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	output.GetDimensions(width, height);
	if (DTid.y >= height / 2.0f)
		output[DTid.xy] = float4(0.4353f, 0.3804f, 0.7529f, 1.0f);
}