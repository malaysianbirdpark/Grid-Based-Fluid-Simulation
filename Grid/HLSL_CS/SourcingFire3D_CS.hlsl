Texture3D<min16float4> velocity_in : register(t0);
Texture3D<min16float4> quantity_in : register(t1);

Texture2DArray<uint> obstacle : register(t10);

RWTexture3D<min16float4> velocity : register(u0);
RWTexture3D<min16float4> quantity : register(u1);

cbuffer color : register(b0) {
	min16float3 dir;
	min16float  speed;
	min16float  quantity_scale;
	min16float  dissipation;
}

cbuffer timestep : register(b1) {
	min16float dt;
}

cbuffer Dimension : register(b3) 
{
    uint width;
    uint height;
    uint depth;
    float reciprocal_width;
    float reciprocal_height;
    float reciprocal_depth;
}

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	velocity[DTid.xyz] = velocity_in[DTid.xyz];
    quantity[DTid.xyz] = quantity_in[DTid.xyz];

    //if (obstacle[DTid.xyz].r <= 0.9f) 
	{
		//const min16float r = min16float(width) * 0.0625f;
		const min16float r = min16float(width) * 0.125f;
		if (DTid.y > height - 5 && DTid.y <= height - 2)
		{
			const min16float x = DTid.x - (min16float(width) * 0.5f);
			const min16float z = DTid.z - (min16float(depth) * 0.5f);

			if (x * x + z * z <= r * r)
			{
				velocity[DTid.xyz] += min16float4(normalize(min16float3(dir.xyz)), 0.0f) * speed;

				static const min16float t_rate = 200.0f;
				static const min16float t_target = 1500.0f;
				const min16float delta_t = (1.0f - exp(-t_rate * dt)) * (t_target - quantity_in[DTid.xyz].g);
				const min16float delta_smoke = 0.0f;
				const min16float delta_f = 59.5f * dt;
				const min16float delta_soot = 0.0f;
				quantity[DTid.xyz] += max(min16float4(delta_smoke, delta_t, delta_f, delta_soot), 0.0f) * quantity_scale;
			}
		}
		else
		{
			const min16float dissipation = exp(-dt * 0.4f);
			//quantity[DTid.xyz] *= min16float4(dissipation, 0.999f, dissipation, dissipation);
			quantity[DTid.xyz] *= dissipation;
		}
    }
}
