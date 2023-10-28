Texture3D<min16float3> velocity_n : register(t0);
Texture3D<unorm float4> quantity_n : register(t1);

Texture3D<min16float3> velocity_n_1_hat : register(t2);
Texture3D<unorm float4> quantity_n_1_hat : register(t3);

Texture3D<min16float3> velocity_n_hat : register(t4);
Texture3D<unorm float4> quantity_n_hat : register(t5);

RWTexture3D<min16float3> velocity_n_1 : register(u0);
RWTexture3D<unorm float4> quantity_n_1 : register(u1);

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

	const min16float3 dr = min16float3((1.0f / width), (1.0f / height), (1.0f / depth));

	const min16float3 pos = (DTid.xyz + 0.5f) * dr;
    const min16float3 target = (pos - velocity_n.SampleLevel(sampler1, pos, 0.0f).xyz * dt);

    min16float3 vels[8];
    vels[0] = velocity_n.SampleLevel(sampler1, target + min16float3(-dr.x, -dr.y, -dr.z), 0.0f).xyz;
    vels[1] = velocity_n.SampleLevel(sampler1, target + min16float3(-dr.x, -dr.y,  dr.z), 0.0f).xyz;
    vels[2] = velocity_n.SampleLevel(sampler1, target + min16float3(-dr.x,  dr.y, -dr.z), 0.0f).xyz;
    vels[3] = velocity_n.SampleLevel(sampler1, target + min16float3(-dr.x,  dr.y,  dr.z), 0.0f).xyz;
    vels[4] = velocity_n.SampleLevel(sampler1, target + min16float3( dr.x, -dr.y, -dr.z), 0.0f).xyz;
    vels[5] = velocity_n.SampleLevel(sampler1, target + min16float3( dr.x, -dr.y,  dr.z), 0.0f).xyz;
    vels[6] = velocity_n.SampleLevel(sampler1, target + min16float3( dr.x,  dr.y, -dr.z), 0.0f).xyz;
    vels[7] = velocity_n.SampleLevel(sampler1, target + min16float3( dr.x,  dr.y,  dr.z), 0.0f).xyz;

    const min16float3 vel_min = min(min(min(min(min(min(min(vels[0], vels[1]), vels[2]), vels[3]), vels[4]), vels[5]), vels[6]), vels[7]);
    const min16float3 vel_max = max(max(max(max(max(max(max(vels[0], vels[1]), vels[2]), vels[3]), vels[4]), vels[5]), vels[6]), vels[7]);
    min16float3 vel_final = velocity_n_1_hat.SampleLevel(sampler1, target, 0.0f) + 0.5 * (velocity_n[DTid.xyz] - velocity_n_hat[DTid.xyz]);
    vel_final = max(min(vel_final, vel_max), vel_min);

    min16float4 q[8];
    q[0] = quantity_n.SampleLevel(sampler1, target + min16float3(-dr.x, -dr.y, -dr.z), 0.0f);
    q[1] = quantity_n.SampleLevel(sampler1, target + min16float3(-dr.x, -dr.y,  dr.z), 0.0f);
    q[2] = quantity_n.SampleLevel(sampler1, target + min16float3(-dr.x,  dr.y, -dr.z), 0.0f);
    q[3] = quantity_n.SampleLevel(sampler1, target + min16float3(-dr.x,  dr.y,  dr.z), 0.0f);
    q[4] = quantity_n.SampleLevel(sampler1, target + min16float3( dr.x, -dr.y, -dr.z), 0.0f);
    q[5] = quantity_n.SampleLevel(sampler1, target + min16float3( dr.x, -dr.y,  dr.z), 0.0f);
    q[6] = quantity_n.SampleLevel(sampler1, target + min16float3( dr.x,  dr.y, -dr.z), 0.0f);
    q[7] = quantity_n.SampleLevel(sampler1, target + min16float3( dr.x,  dr.y,  dr.z), 0.0f);

    const min16float4 quantity_min = min(min(min(min(min(min(min(q[0], q[1]), q[2]), q[3]), q[4]), q[5]), q[6]), q[7]);
    const min16float4 quantity_max = max(max(max(max(max(max(max(q[0], q[1]), q[2]), q[3]), q[4]), q[5]), q[6]), q[7]);
    min16float4 quantity_final = quantity_n_1_hat.SampleLevel(sampler1, target, 0.0f) + 0.5 * (quantity_n[DTid.xyz] - quantity_n_hat[DTid.xyz]);
    quantity_final = max(min(quantity_final, quantity_max), quantity_min);

    velocity_n_1[DTid.xyz] = vel_final;
    quantity_n_1[DTid.xyz] = quantity_final;
}