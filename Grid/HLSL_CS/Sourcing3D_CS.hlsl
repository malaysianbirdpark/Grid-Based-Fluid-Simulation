Texture3D<float3> velocity_in : register(s0);
Texture3D<float4> quantity_in : register(s1);

RWTexture3D<float3> velocity : register(u0);
RWTexture3D<float4> quantity : register(u1);

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

	velocity[DTid.xyz].xyz = velocity_in[DTid.xyz].xyz;
    quantity[DTid.xyz] = quantity_in[DTid.xyz];

	//const float x = DTid.x - (width / 2);
	//const float y = DTid.y - (height / 2);
	//const float z = DTid.z - (depth / 2);
	//const float r = 50.0f;

	//if (DTid.x < width && DTid.y < height && DTid.z < depth) {
 //       if (x * x + y * y + z * z <= r * r) {
 //           velocity[DTid.xyz].xyz += normalize(float3(dir.xyz)) * speed;
 //           quantity[DTid.xyz] += color * color_scale;
 //       }
	//}

	if (DTid.x <= 20.0f && DTid.y >= 50.0f && DTid.y <= 60.0f) {
		velocity[DTid.xyz].xyz += normalize(float3(dir.xyz)) * speed;
		quantity[DTid.xyz] += color * color_scale;
	}
}