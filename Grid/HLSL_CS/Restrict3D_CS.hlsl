Texture3D<min16float> finer;

RWTexture3D<min16float> coarser;

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	const uint x = DTid.x << 1;
	const uint y = DTid.y << 1;
	const uint z = DTid.z << 1;

	min16float p[8];
	p[0] = finer[uint3(x    , y    , z    )];
	p[1] = finer[uint3(x + 1, y    , z    )];
	p[2] = finer[uint3(x    , y + 1, z    )];
	p[3] = finer[uint3(x + 1, y + 1, z    )];
	p[4] = finer[uint3(x    , y    , z + 1)];
	p[5] = finer[uint3(x + 1, y    , z + 1)];
	p[6] = finer[uint3(x    , y + 1, z + 1)];
	p[7] = finer[uint3(x + 1, y + 1, z + 1)];

	coarser[DTid.xyz] = (p[0] + p[1] + p[2] + p[3] + p[4] + p[5] + p[6] + p[7]) * 0.125f;
}