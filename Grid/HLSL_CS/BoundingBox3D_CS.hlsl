Texture2DArray<uint4> stencil : register(t0);
RWTexture2DArray<unorm float> obstacle : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	uint depth;
	obstacle.GetDimensions(width, height, depth);
	
	obstacle[DTid.xyz] = 0.0f;
	if (stencil[DTid.xyz].g > 0.9f || DTid.x == 0 || DTid.x == width - 1 || DTid.y == 0 || DTid.y == height - 1 || DTid.z == 0 || DTid.z == depth - 1)
		obstacle[DTid.xyz] = 1.0f;
}