Texture3D<min16float3> velocity_in : register(t0);
Texture3D              quantity_in : register(t1);

RWTexture3D<min16float3> velocity_out : register(u0);
RWTexture3D<float4>      quantity_out : register(u1);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);

cbuffer constants : register(b1) {
	min16float dt;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID ) {
	uint width;
	uint height;
	uint depth;
	quantity_in.GetDimensions(width, height, depth);

	//if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1 && DTid.z > 0 && DTid.z < depth - 1) 
	{
		const min16float3 dr = min16float3((1.0f / width), (1.0f / height), (1.0f / depth));

		const min16float3 pos    = (DTid.xyz + 0.5f) * dr;
		const min16float3 mid    = pos - 0.5f * velocity_in.SampleLevel(sampler1, pos, 0.0f).xyz * dt;
		const min16float3 target = pos - velocity_in.SampleLevel(sampler1, mid, 0.0f).xyz * dt;

		velocity_out[DTid.xyz] = velocity_in.SampleLevel(sampler0, target, 0.0f);
		quantity_out[DTid.xyz] = quantity_in.SampleLevel(sampler0, target, 0.0f);
	}
}