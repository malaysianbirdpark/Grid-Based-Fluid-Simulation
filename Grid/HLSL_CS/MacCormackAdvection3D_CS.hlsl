Texture3D<min16float3> velocity_n : register(t0);
Texture3D<min16float2> quantity_n : register(t1);

Texture3D<min16float3> velocity_n_1_hat : register(t2);
Texture3D<min16float2> quantity_n_1_hat : register(t3);

Texture3D<min16float3> velocity_n_hat : register(t4);
Texture3D<min16float2> quantity_n_hat : register(t5);

Texture2DArray obstacle     : register(t6);
Texture2DArray obstacle_vel : register(t7);

RWTexture3D<min16float3> velocity_n_1 : register(u0);
RWTexture3D<min16float2> quantity_n_1 : register(u1);

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

	if (obstacle[DTid.xyz].r <= 0.9f) {
		min16float3 dr = min16float3((1.0f / width), (1.0f / height), (1.0f / depth));

		min16float3 pos = min16float3(DTid.xyz);
		min16float3 k1 = pos - velocity_n.SampleLevel(sampler0, (pos + 0.5f) * dr, 0.0f).xyz * dt;
		k1 = (k1 + 0.5f) * dr;
		min16float3 k2 = pos - 0.5f  * velocity_n.SampleLevel(sampler0, k1, 0.0f).xyz * dt;
		k2 = (k2 + 0.5f) * dr;
		min16float3 k3 = pos - 0.75f * velocity_n.SampleLevel(sampler0, k2, 0.0f).xyz * dt;
		k3 = (k3 + 0.5f) * dr;

		const min16float3 target =
			((pos - (0.2222222f * velocity_n.SampleLevel(sampler0, k1, 0.0f).xyz +
			     	 0.3333333f * velocity_n.SampleLevel(sampler0, k2, 0.0f).xyz +
				     0.4444444f * velocity_n.SampleLevel(sampler0, k3, 0.0f).xyz) * dt) + 0.5f) * dr;
        pos = (pos + 0.5f) * dr;

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
			0.5f * (velocity_n.SampleLevel(sampler1, pos, 0.0f) - velocity_n_hat.SampleLevel(sampler1, pos, 0.0f));
        vel_final = max(min(vel_final, vel_max), vel_min);

        min16float2 q[8];
        q[0] = quantity_n.SampleLevel(sampler0, target + min16float3(-dr.x, -dr.y, -dr.z), 0.0f);
        q[1] = quantity_n.SampleLevel(sampler0, target + min16float3(-dr.x, -dr.y,  dr.z), 0.0f);
        q[2] = quantity_n.SampleLevel(sampler0, target + min16float3(-dr.x,  dr.y, -dr.z), 0.0f);
        q[3] = quantity_n.SampleLevel(sampler0, target + min16float3(-dr.x,  dr.y,  dr.z), 0.0f);
        q[4] = quantity_n.SampleLevel(sampler0, target + min16float3( dr.x, -dr.y, -dr.z), 0.0f);
        q[5] = quantity_n.SampleLevel(sampler0, target + min16float3( dr.x, -dr.y,  dr.z), 0.0f);
        q[6] = quantity_n.SampleLevel(sampler0, target + min16float3( dr.x,  dr.y, -dr.z), 0.0f);
        q[7] = quantity_n.SampleLevel(sampler0, target + min16float3( dr.x,  dr.y,  dr.z), 0.0f);

        const min16float2 q_min = min(min(min(min(min(min(min(q[0], q[1]), q[2]), q[3]), q[4]), q[5]), q[6]), q[7]);
        const min16float2 q_max = max(max(max(max(max(max(max(q[0], q[1]), q[2]), q[3]), q[4]), q[5]), q[6]), q[7]);
		min16float2 q_final = quantity_n_1_hat.SampleLevel(sampler0, target, 0.0f) +
			0.5f * (quantity_n.SampleLevel(sampler1, pos, 0.0f) - quantity_n_hat.SampleLevel(sampler1, pos, 0.0f));
        q_final = max(min(q_final, q_max), q_min);

		velocity_n_1[DTid.xyz] = vel_final;
        quantity_n_1[DTid.xyz] = q_final;
	}
    else {
        velocity_n_1[DTid.xyz] = min16float3(-obstacle_vel[DTid.xyz].x, -obstacle_vel[DTid.xyz].y, -obstacle_vel[DTid.xyz].z) * 2.2f;
        if (length(velocity_n_1[DTid.xyz]) <= 1e-5)
            quantity_n_1[DTid.xyz] = 0.0f;
    }
}