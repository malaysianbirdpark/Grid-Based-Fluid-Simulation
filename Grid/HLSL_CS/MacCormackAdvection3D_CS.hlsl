Texture3D<min16float3> velocity_n : register(t0);
Texture3D<min16float>  density_n : register(t1);

Texture3D<min16float3> velocity_n_1_hat : register(t2);
Texture3D<min16float>  density_n_1_hat : register(t3);

Texture3D<min16float3> velocity_n_hat : register(t4);
Texture3D<min16float>  density_n_hat : register(t5);

RWTexture3D<min16float3> velocity_n_1 : register(u0);
RWTexture3D<min16float>  density_n_1 : register(u1);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);

cbuffer constants : register(b1) {
	min16float dt;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID ) {
    uint width;
    uint height;
    uint depth;
    velocity_n.GetDimensions(width, height, depth);

	if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1 && DTid.z > 0 && DTid.z < depth - 1) {
		min16float3 dr = min16float3((1.0f / width), (1.0f / height), (1.0f / depth));

		const min16float3 pos    = (DTid.xyz + 0.5f) * dr;
		const min16float3 mid    = pos - 0.5f * velocity_n.SampleLevel(sampler1, pos, 0.0f).xyz * dt;
		const min16float3 target = pos - velocity_n.SampleLevel(sampler1, mid, 0.0f).xyz * dt;
		//const min16float3 target = pos - velocity_n.SampleLevel(sampler1, pos, 0.0f).xyz * dt;

        dr *= 0.5f;

        min16float3 vels[8];
        vels[0] = velocity_n.SampleLevel(sampler0, target + min16float3(-dr.x, -dr.y, -dr.z), 0.0f);
        vels[1] = velocity_n.SampleLevel(sampler0, target + min16float3(-dr.x, -dr.y,  dr.z), 0.0f);
        vels[2] = velocity_n.SampleLevel(sampler0, target + min16float3(-dr.x,  dr.y, -dr.z), 0.0f);
        vels[3] = velocity_n.SampleLevel(sampler0, target + min16float3(-dr.x,  dr.y,  dr.z), 0.0f);
        vels[4] = velocity_n.SampleLevel(sampler0, target + min16float3( dr.x, -dr.y, -dr.z), 0.0f);
        vels[5] = velocity_n.SampleLevel(sampler0, target + min16float3( dr.x, -dr.y,  dr.z), 0.0f);
        vels[6] = velocity_n.SampleLevel(sampler0, target + min16float3( dr.x,  dr.y, -dr.z), 0.0f);
        vels[7] = velocity_n.SampleLevel(sampler0, target + min16float3( dr.x,  dr.y,  dr.z), 0.0f);

        const min16float3 vel_min = min(min(min(min(min(min(min(vels[0], vels[1]), vels[2]), vels[3]), vels[4]), vels[5]), vels[6]), vels[7]);
        const min16float3 vel_max = max(max(max(max(max(max(max(vels[0], vels[1]), vels[2]), vels[3]), vels[4]), vels[5]), vels[6]), vels[7]);
		min16float3 vel_final = velocity_n_1_hat.SampleLevel(sampler0, target, 0.0f) +
			0.5f * (velocity_n.SampleLevel(sampler0, pos, 0.0f) - velocity_n_hat.SampleLevel(sampler0, pos, 0.0f));
        vel_final = max(min(vel_final, vel_max), vel_min);

        min16float d[8];
        d[0] = density_n.SampleLevel(sampler0, target + min16float3(-dr.x, -dr.y, -dr.z), 0.0f);
        d[1] = density_n.SampleLevel(sampler0, target + min16float3(-dr.x, -dr.y,  dr.z), 0.0f);
        d[2] = density_n.SampleLevel(sampler0, target + min16float3(-dr.x,  dr.y, -dr.z), 0.0f);
        d[3] = density_n.SampleLevel(sampler0, target + min16float3(-dr.x,  dr.y,  dr.z), 0.0f);
        d[4] = density_n.SampleLevel(sampler0, target + min16float3( dr.x, -dr.y, -dr.z), 0.0f);
        d[5] = density_n.SampleLevel(sampler0, target + min16float3( dr.x, -dr.y,  dr.z), 0.0f);
        d[6] = density_n.SampleLevel(sampler0, target + min16float3( dr.x,  dr.y, -dr.z), 0.0f);
        d[7] = density_n.SampleLevel(sampler0, target + min16float3( dr.x,  dr.y,  dr.z), 0.0f);

        const min16float3 d_min = min(min(min(min(min(min(min(d[0], d[1]), d[2]), d[3]), d[4]), d[5]), d[6]), d[7]);
        const min16float3 d_max = max(max(max(max(max(max(max(d[0], d[1]), d[2]), d[3]), d[4]), d[5]), d[6]), d[7]);
		min16float3 density_final = density_n_1_hat.SampleLevel(sampler0, target, 0.0f) +
			0.5f * (density_n.SampleLevel(sampler0, pos, 0.0f) - density_n_hat.SampleLevel(sampler0, pos, 0.0f));
        vel_final = max(min(vel_final, vel_max), vel_min);

		velocity_n_1[DTid.xyz] = vel_final;
        density_n_1[DTid.xyz] = density_final;
	}
}