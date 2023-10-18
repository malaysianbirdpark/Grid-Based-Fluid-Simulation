RWTexture2D<float2> velocity_in : register(u0);
RWTexture2D<float4> quantity_in : register(u1);

RWTexture2D<float2> velocity : register(u2);
RWTexture2D<float4> quantity : register(u3);

cbuffer color : register(b0) {
	float4 fluid_color;
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID )
{
	velocity[DTid.xy] += float2(0.01f, 0.0f);
	if (DTid.x >= 100.0f && DTid.x <= 150.0f && DTid.y >= 200.0f && DTid.y <= 250.0f) {
		velocity[DTid.xy] = velocity_in[DTid.xy] + float2(0.1f, 0.0f);
		quantity[DTid.xy] = quantity_in[DTid.xy] + fluid_color;
	}
}