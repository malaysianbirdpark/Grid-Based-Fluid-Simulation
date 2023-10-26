RWTexture3D<float4> target : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID ) {
	uint width;
	uint height;
	uint depth;
	target.GetDimensions(width, height, depth);

	if (DTid.z >= 200 && DTid.z <= 300)
		target[DTid.xyz] = float4(0.0f, 1.0f, 0.0f, 0.05f);
	else
		target[DTid.xyz] = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//if (DTid.z >= 200 && DTid.z <= 300)
	//	target[DTid.xyz] = float4(0.0f, 1.0f, 0.0f, 0.05f);
	//else if (DTid.x <= 300)
	//	target[DTid.xyz] = float4(0.0f, 0.0f, 1.0f, 0.04f);
	//else
	//	target[DTid.xyz] = float4(1.0f, 0.0f, 0.0f, 0.02f);

	//const float x = DTid.x - (width / 2);
	//const float y = DTid.y - (height / 2);
	//const float z = DTid.z - (depth / 2);
	//const float r = 100.0f;

	//if (x * x + y * y + z * z <= r * r)
	//	target[DTid.xyz] = float4(0.0f, 0.0f, 1.0f, 1.0f);
	//else
	//	target[DTid.xyz] = float4(0.0f, 0.0f, 0.0f, 0.0f);
}