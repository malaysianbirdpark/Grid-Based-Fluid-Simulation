RWTexture3D<float4> target : register(u0);

[numthreads(16, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID ) {
	uint width;
	uint height;
	uint depth;
	target.GetDimensions(width, height, depth);

	//if ((DTid.x - (width / 2)) * (DTid.x - (width / 2)) + (DTid.y - (height / 2)) * (DTid.y - (height / 2)) + (DTid.z - (20)) * (DTid.z - (20)) <= 100.0f) {
	//	target[DTid.xyz] = float4(0.0f, 0.0f, 1.0f, 1.0f);
	//}
	//else if ((DTid.x - (width / 2)) * (DTid.x - (width / 2)) + (DTid.y - (height / 2)) * (DTid.y - (height / 2)) + (DTid.z - (90)) * (DTid.z - (90)) <= 400.0f) {
	//	target[DTid.xyz] = float4(1.0f, 0.0f, 0.0f, 0.5f);
	//}
	//else if ((DTid.x - (width / 2)) * (DTid.x - (width / 2)) + (DTid.y - (height / 2)) * (DTid.y - (height / 2)) + (DTid.z - (400)) * (DTid.z - (400)) <= 900.0f) {
	//	target[DTid.xyz] = float4(0.0f, 1.0f, 0.0f, 0.25f);
	//}

	//if (DTid.x >= width / 2) {
	//	if (DTid.z >= depth / 2) {
	//		target[DTid.xyz] = float4(0.0f, 1.0f, 0.0f, 0.1f);
	//	}
	//	else {
	//		target[DTid.xyz] = float4(0.0f, 0.0f, 1.0f, 0.2f);
	//	}
	//}
	//else {
	//	target[DTid.xyz] = float4(1.0f, 0.0f, 0.0f, 0.2f);
	//}

	if ((DTid.x - (width / 2)) * (DTid.x - (width / 2)) + (DTid.y - (height / 2)) * (DTid.y - (height / 2)) + (DTid.z - (depth / 2)) * (DTid.z - (depth / 2)) <= 64000.0f)
		target[DTid.xyz] = float4(0.0f, 0.0f, 1.0f, 1.0f);
	else
		target[DTid.xyz] = float4(0.0f, 0.0f, 0.0f, 0.0f);
}