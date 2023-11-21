Texture2DArray<uint4> stencil : register(t0);
RWTexture2DArray<uint>    obstacle     : register(u0);
RWTexture2DArray<float4>  obstacle_vel : register(u1);

cbuffer Dimension : register(b3) 
{
    uint width;
    uint height;
    uint depth;
    float reciprocal_width;
    float reciprocal_height;
    float reciprocal_depth;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	obstacle[DTid.xyz] = 0;
	if (stencil[DTid.xyz].g > 0.9f) {
		obstacle[DTid.xyz] = 2;
	}
	else if (DTid.x == 0 || DTid.x == width - 1 || DTid.y == 0 || DTid.y == height - 1 || DTid.z == 0 || DTid.z == depth - 1) {
		obstacle[DTid.xyz] = 1;
		
		obstacle_vel[DTid.xyz] = float4(0.0f, -0.15f, 0.0f, 0.0f);
	}
}