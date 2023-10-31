Texture3D<min16float3> velocity_in : register(t0);
Texture3D<unorm float4> quantity_in : register(t1);

RWTexture3D<min16float3> velocity : register(u0);
RWTexture3D<unorm float4> quantity : register(u1);

cbuffer color : register(b0) {
	float3 dir;
	float  speed;
	float4 color;
	float  color_scale;
	float  dissipation;
}

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	uint depth;
	quantity.GetDimensions(width, height, depth);

	velocity[DTid.xyz] = velocity_in[DTid.xyz];
	quantity[DTid.xyz] = quantity_in[DTid.xyz];

	const min16float r = min16float(width) / 32;
	if (DTid.y > height - 32 && DTid.y <= height - 30) {
		const min16float x = DTid.x - (min16float(width) / 2.0f);
		const min16float z = DTid.z - (min16float(depth) / 2.0f);

		if (x * x + z * z <= r * r) {
			velocity[DTid.xyz].xyz = normalize(min16float3(dir.xyz)) * speed;
			quantity[DTid.xyz] = min16float4(color.xyz * color_scale, 0.3f);
		}
	}

	//if (DTid.y > 124.0f && DTid.y <= 126.0f && DTid.z >= 50.0f && DTid.z <= 70.0f && DTid.x >= 50.0f && DTid.x <= 70.0f) {
	//	if (DTid.x >= 60 && DTid.x <= 65 && DTid.z >= 60 && DTid.z <= 65) {
	//		velocity[DTid.xyz].xyz += normalize(min16float3(dir.xyz)) * speed * 2.0f;
	//	}
	//	else {
	//		velocity[DTid.xyz].xyz += normalize(min16float3(dir.xyz)) * speed * 0.5f;
	//	}
	//	quantity[DTid.xyz] += min16float4(color.xyz * color_scale, 0.2f);
	//}

	//if (length(velocity[DTid.xyz].xyz) < 0.03f)
	//	quantity[DTid.xyz] = 0.0f;
		//quantity[DTid.xyz] = clamp(quantity[DTid.xyz] - dissipation, 0.0f, 1.0f);
}