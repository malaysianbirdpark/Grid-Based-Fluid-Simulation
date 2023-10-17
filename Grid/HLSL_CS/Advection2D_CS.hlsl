Texture2D velocity_in : register(t0);
Texture2D quantity_in : register(t1);

RWTexture2D<float2> velocity_out : register(u0);
RWTexture2D<float4> quantity_out : register(u1);

SamplerState sampler0 : register(s0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
	uint width;
	uint height;
	quantity_in.GetDimensions(width, height);

	// Assuming square
	const float2 dx = float2((1.0f / width), (1.0f / height));
	const float dt = 0.1f;

    //const float2 pos = DTid.xy * dx - velocity_in.SampleLevel(sampler0, DTid.xy, 0.0f) * dt;
    float2 pos = (DTid.xy + 0.5f) * dx;
    pos -= float2(1.0f, 0.0f) * dt;

    velocity_out[DTid.xy] = float2(1.0f, 0.0f);
    //velocity_out[DTid.xy] = velocity_in.SampleLevel(sampler0, pos, 0.0f);
	quantity_out[DTid.xy] = quantity_in.SampleLevel(sampler0, pos, 0.0f);
    //quantity_out[DTid.xy] = float4(velocity_out[DTid.xy].xy, 0.0f, 1.0f);
}