Texture3D<float3> velocity_in : register(t0);
Texture3D<float4> quantity_in : register(t1);

RWTexture3D<float3> velocity_out : register(u0);
RWTexture3D<float4> quantity_out : register(u1);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);

cbuffer constants : register(b1) {
	float dt;
}

[numthreads(16, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID ) {
	uint width;
	uint height;
	uint depth;
	quantity_in.GetDimensions(width, height, depth);

	const float3 dr = float3((1.0f / width), (1.0f / height), (1.0f / depth));

	const float3 pos = (DTid.xyz + 0.5f) * dr;
    const float3 target = (pos - velocity_in.SampleLevel(sampler1, pos, 0.0f).xyz * dt);

    velocity_out[DTid.xyz].xyz = velocity_in.SampleLevel(sampler0, target, 0.0f).xyz;
	quantity_out[DTid.xyz] = quantity_in.SampleLevel(sampler0, target, 0.0f);
}