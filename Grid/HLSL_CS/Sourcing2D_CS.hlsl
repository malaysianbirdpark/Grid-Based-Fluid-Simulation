RWTexture2D<float2> velocity : register(u0);
RWTexture2D<float4> quantity : register(u1);

cbuffer color : register(b0) {
	float4 fluid_color;
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID )
{
	if (DTid.x <= 300)
		quantity[DTid.xy] = fluid_color;
}