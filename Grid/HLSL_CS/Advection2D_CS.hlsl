Texture2D velocity_in : register(t0);
Texture2D quantity_in : register(t1);

RWTexture2D<float2> velocity_out : register(u0);
RWTexture2D<float4> quantity_out : register(u1);

SamplerState sampler0 : register(s0);

cbuffer constants : register(b1) {
	float dt;
}

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID ) {
	uint width;
	uint height;
	quantity_in.GetDimensions(width, height);

	// Assuming square
	const float2 dr = float2((1.0f / width), (1.0f / height));

    float2 pos = (DTid.xy + 0.5f) * dr;
    pos -= velocity_in.SampleLevel(sampler0, pos, 0.0f) * dt;

    velocity_out[DTid.xy] = velocity_in.SampleLevel(sampler0, pos, 0.0f);
	quantity_out[DTid.xy] = quantity_in.SampleLevel(sampler0, pos, 0.0f);
}