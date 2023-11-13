Texture3D<min16float3> velocity_in : register(t0);
Texture3D<min16float2> quantity_in : register(t1);

Texture2DArray<uint> obstacle : register(t6);

RWTexture3D<min16float3> velocity : register(u0);
RWTexture3D<min16float2> quantity : register(u1);

cbuffer color : register(b0) {
	min16float3 dir;
	min16float  speed;
	min16float  quantity_scale;
	min16float  dissipation;
}

cbuffer timestep : register(b1) {
	min16float dt;
}

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint width;
	uint height;
	uint depth;
	velocity_in.GetDimensions(width, height, depth);

	velocity[DTid.xyz] = velocity_in[DTid.xyz];
    quantity[DTid.xyz] = quantity_in[DTid.xyz];

    //if (obstacle[DTid.xyz].r <= 0.9f) 
	{
		const min16float r = min16float(width) * 0.25f;
		if (DTid.y > height - 5 && DTid.y <= height - 2)
		{
			const min16float x = DTid.x - (min16float(width) * 0.5f);
			const min16float z = DTid.z - (min16float(depth) * 0.5f);

			if (x * x + z * z <= r * r)
			{
				velocity[DTid.xyz] += normalize(min16float3(dir.xyz)) * speed;

				static const min16float t_rate = 1000.0f;
				static const min16float t_target = 630.0f;
				const min16float delta_t = (1.0f - exp(-t_rate * dt)) * (t_target - quantity[DTid.xyz]);
				const min16float delta_s = 1500.0f * dt;
				quantity[DTid.xyz] += min16float2(delta_s, delta_t) * quantity_scale;
			}
		}
		else
		{
			quantity[DTid.xyz] *= exp(-dt * 0.4f);
		}
    }
}
