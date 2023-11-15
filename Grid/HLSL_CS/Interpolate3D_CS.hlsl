Texture3D<min16float> coarser;

RWTexture3D<min16float> finer;

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	const uint x = DTid.x >> 1;
	const uint y = DTid.y >> 1;
	const uint z = DTid.z >> 1;

	min16float p[8];
	p[0] = coarser[uint3(x + 1, y + 1, z + 1)];
	p[1] = coarser[uint3(x + 1, y + 1, z    )];
	p[2] = coarser[uint3(x + 1, y    , z + 1)];
	p[3] = coarser[uint3(x    , y + 1, z + 1)];
	p[4] = coarser[uint3(x + 1, y    , z    )];
	p[5] = coarser[uint3(x    , y + 1, z    )];
	p[6] = coarser[uint3(x    , y    , z + 1)];
	p[7] = coarser[uint3(x    , y    , z    )];

	finer[DTid.xyz] = (p[0] * 0.015625f) + ((p[1] + p[2] + p[3]) * 0.046875f) + ((p[4] + p[5] + p[6]) * 0.140625f) + (p[7] * 0.421875f);
}