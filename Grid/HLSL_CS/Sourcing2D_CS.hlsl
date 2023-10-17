RWTexture2D<float2> velocity : register(u0);
RWTexture2D<float4> quantity : register(u1);

cbuffer color : register(b0) {
	float4 fluid_color;
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID )
{
    velocity[DTid.xy] = float2(1.0f, 0.0f);
	if (DTid.x >= 100.0f && DTid.x <= 150.0f && DTid.y >= 200.0f && DTid.y <= 250.0f) {
		quantity[DTid.xy] += float4(0.1f, 0.0f, 0.0f, 1.0f);
	}
}