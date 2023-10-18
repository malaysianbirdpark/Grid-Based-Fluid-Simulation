RWTexture2D<float2> velocity : register(u0);
RWTexture2D<float4> quantity : register(u1);

cbuffer color : register(b0) {
	float4 fluid_color;
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID )
{
	if (DTid.x >= 100.0f && DTid.x <= 150.0f && DTid.y >= 200.0f && DTid.y <= 250.0f) {
		velocity[DTid.xy] = float2(0.01f, 0.0f);
		quantity[DTid.xy] += fluid_color * 0.1f;
	}
}