Texture2D<float3> velocity_in : register(s0);
Texture2D<float4> quantity_in : register(s1);

RWTexture2D<float3> velocity : register(u0);
RWTexture2D<float4> quantity : register(u1);

cbuffer color : register(b0) {
	float3 dir;
	float  color_scale;
	float  speed;
	float4 color;
}

[numthreads(16, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	uint depth;
	quantity_in.GetDimensions(width, height, depth);

	velocity[DTid.xyz].xyz = velocity_in[DTid.xyz].xyz;
    quantity[DTid.xyz] = quantity_in[DTid.xyz];

	// circle at the center
	if (DTid.x < width && DTid.y < height && DTid.z < depth) {
        if ((DTid.x - (width / 2)) * (DTid.x - (width / 2)) + (DTid.y - (height / 2)) * (DTid.y - (height / 2)) + (DTid.z - (depth / 2)) * (DTid.z - (depth / 2)) <= 1600) {
            velocity[DTid.xy].xyz += normalize(float3(dir.xyz)) * speed;
            quantity[DTid.xy] += color * color_scale;
        }
	}
}