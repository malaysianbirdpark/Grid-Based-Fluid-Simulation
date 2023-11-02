Texture3D<min16float4> velocity_in : register(t0);

RWTexture3D<min16float4> velocity : register(u0);

cbuffer color : register(b0) {
	min16float3 dir;
	min16float  speed;
	min16float  density_scale;
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

	const min16float r = min16float(width) / 32;
	if (DTid.y > height - 32 && DTid.y <= height - 30) {
		const min16float x = DTid.x - (min16float(width) / 2.0f);
		const min16float z = DTid.z - (min16float(depth) / 2.0f);

		if (x * x + z * z <= r * r)
			//velocity[DTid.xyz] = min16float4(normalize(min16float3(dir.xyz)) * speed, density_scale);
			velocity[DTid.xyz] = min16float4(normalize(min16float3(dir.xyz)) * speed, 20000.0f);
	}
}
