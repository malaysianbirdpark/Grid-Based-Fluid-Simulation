Texture3D<min16float3> velocity_in : register(t0);
Texture3D<min16float2> quantity_in : register(t1);

Texture2DArray obstacle : register(t6);

RWTexture3D<min16float3> velocity : register(u0);
RWTexture3D<min16float2> quantity : register(u1);

cbuffer color : register(b0) {
	min16float3 dir;
	min16float  speed;
	min16float  quantity_scale;
	min16float  dissipation;
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

    if (obstacle[DTid.xyz].r <= 0.9f) {
		const min16float r = min16float(width) / 8;
		if (DTid.y > height - 4 && DTid.y <= height - 2)
		{
			const min16float x = DTid.x - (min16float(width) / 2.0f);
			const min16float z = DTid.z - (min16float(depth) / 2.0f);

			if (x * x + z * z <= r * r)
			{
				velocity[DTid.xyz] += normalize(min16float3(dir.xyz)) * speed;
				quantity[DTid.xyz] += min16float2(10.0f, 880.0f);
			}
		}
		else
		{
			quantity[DTid.xyz] = max(quantity_in[DTid.xyz] - min16float2(0.0025f, 0.007f), 0.0f);
		}
    }
}
