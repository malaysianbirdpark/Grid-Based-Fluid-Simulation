RWTexture2D<float2> velocity : register(u0);
RWTexture2D<float4> quantity : register(u1);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	if (DTid.x >= 400)
		quantity[DTid.xy] = float4(0.5f, 1.0f, 0.25f, 1.0f);
}