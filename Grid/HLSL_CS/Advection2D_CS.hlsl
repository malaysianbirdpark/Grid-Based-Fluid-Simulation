Texture2D<float2> velocity_in : register(t0);
Texture2D<float4> quantity_in : register(t1);

RWTexture2D<float2> velocity_out : register(u0);
RWTexture2D<float4> quantity_out : register(u1);

SamplerState sampler0 : register(s0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
	uint width;
	uint height;
	velocity_in.GetDimensions(width, height);

	// Assuming square
	const float dx = (1.0f / width);
	const float dt = 0.5f;

	const float2 pos = DTid.xy - velocity_in.SampleLevel(sampler0, DTid.xy, 0.0f) * dx * dt;

	velocity_out[DTid.xy] = velocity_in.SampleLevel(sampler0, pos, 0.0f) * dx * dt;
	quantity_out[DTid.xy] = quantity_in.SampleLevel(sampler0, pos, 0.0f) * dx * dt;
}