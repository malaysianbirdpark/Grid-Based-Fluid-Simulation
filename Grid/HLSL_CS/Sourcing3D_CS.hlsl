Texture3D<min16float3> velocity_in : register(s0);
Texture3D<unorm float4> quantity_in : register(s1);

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

	if (DTid.y > 2.0f && DTid.y <= 3.0f && DTid.z >= 50.0f && DTid.z <= 70.0f && DTid.x >= 50.0f && DTid.x <= 70.0f) {
		velocity[DTid.xyz].xyz = normalize(min16float3(dir.xyz)) * speed;
		quantity[DTid.xyz] = min16float4(color.xyz * color_scale, 0.4f);
	}
    quantity[DTid.xyz] -= dissipation;

	if (velocity[DTid.xyz].x <= 0.001f && velocity[DTid.xyz].y <= 0.001f && velocity[DTid.xyz].z <= 0.001f) {
        quantity[DTid.xyz] = 0.0f;
    }
}