Texture3D<min16float3> velocity_n : register(t0);
Texture3D<min16float2>  quantity_n : register(t1);

Texture2DArray obstacle : register(t6);

RWTexture3D<min16float3> velocity_n_1 : register(u0);
RWTexture3D<min16float2>  quantity_n_1  : register(u1);

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
	velocity_n.GetDimensions(width, height, depth);

	if (obstacle[DTid.xyz].r <= 0.9f) {
		const min16float3 dr = min16float3((1.0f / width), (1.0f / height), (1.0f / depth));

		const min16float3 pos = min16float3(DTid.xyz);
		min16float3 k1 = pos + velocity_n.SampleLevel(sampler0, (pos + 0.5f) * dr, 0.0f) * dt;
		k1 = (k1 + 0.5f) * dr;
		min16float3 k2 = pos + 0.5f  * velocity_n.SampleLevel(sampler0, k1, 0.0f) * dt;
		k2 = (k2 + 0.5f) * dr;
		min16float3 k3 = pos + 0.75f * velocity_n.SampleLevel(sampler0, k2, 0.0f) * dt;
		k3 = (k3 + 0.5f) * dr;

		const min16float3 target =
			((pos + (0.2222222f * velocity_n.SampleLevel(sampler0, k1, 0.0f) +
			 	     0.3333333f * velocity_n.SampleLevel(sampler0, k2, 0.0f) +
				     0.4444444f * velocity_n.SampleLevel(sampler0, k3, 0.0f)) * dt) + 0.5f) * dr;

		velocity_n_1[DTid.xyz] = velocity_n.SampleLevel(sampler0, target, 0.0f);
		quantity_n_1[DTid.xyz] = quantity_n.SampleLevel(sampler0, target, 0.0f);
	}
}