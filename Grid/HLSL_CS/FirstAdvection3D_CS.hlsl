Texture3D<min16float3> velocity_in : register(t0);
Texture3D<min16float> density_in : register(t1);

RWTexture3D<min16float3> velocity_out : register(u0);
RWTexture3D<min16float> density_out : register(u1);

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
	velocity_in.GetDimensions(width, height, depth);

	//if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1 && DTid.z > 0 && DTid.z < depth - 1) 
	{
		const min16float3 dr = min16float3((1.0f / width), (1.0f / height), (1.0f / depth));

		const min16float3 pos    = (DTid.xyz + 0.5f) * dr;
		const min16float3 k1     = pos - velocity_in.SampleLevel(sampler0, pos, 0.0f).xyz * dt;
		const min16float3 k2     = pos - 0.5f  * velocity_in.SampleLevel(sampler0, k1, 0.0f).xyz * dt;
		const min16float3 k3     = pos - 0.75f * velocity_in.SampleLevel(sampler0, k2, 0.0f).xyz * dt;
		const min16float3 target =
			pos - (0.2222222f * velocity_in.SampleLevel(sampler0, k1, 0.0f).xyz +
				   0.3333333f * velocity_in.SampleLevel(sampler0, k2, 0.0f).xyz +
				   0.4444444f * velocity_in.SampleLevel(sampler0, k3, 0.0f).xyz) * dt;

		velocity_out[DTid.xyz] = velocity_in.SampleLevel(sampler0, target, 0.0f);
		density_out[DTid.xyz]  = density_in.SampleLevel(sampler0, target, 0.0f);
	}
}