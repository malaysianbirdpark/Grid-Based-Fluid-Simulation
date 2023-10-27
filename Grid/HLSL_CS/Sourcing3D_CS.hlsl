Texture3D<min16float3> velocity_in : register(s0);
Texture3D<unorm float4> quantity_in : register(s1);

RWTexture3D<min16float3> velocity : register(u0);
RWTexture3D<unorm float4> quantity : register(u1);

cbuffer color : register(b0) {
	float3 dir;
	float  speed;
	float4 color;
	float  color_scale;
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

	if (DTid.x == 0 || DTid.y == 0 || DTid.z == 0) {
		//velocity[DTid.xyz] = 0.0f;
	}
	else if (DTid.x >= width - 1 || DTid.y >= height - 1 || DTid.z >= depth - 1) {
		velocity[DTid.xyz] = min16float3(-2.0f, 5.0f, 0.0f);
	}
	else if (DTid.x > 0.0f && DTid.x <= 4.0f && DTid.y >= 10.0f && DTid.y <= 20.0f) {
		//velocity[DTid.xyz].xyz += normalize(float3(dir.xyz)) * speed;
		velocity[DTid.xyz] += min16float3(1.0f, 0.0f, 0.0f) * 0.15f;
		//quantity[DTid.xyz] += color * color_scale;
		quantity[DTid.xyz] += float4(1.0f, 1.0f, 1.0f, 0.3f) * 0.1f;
	}
}