Texture3D<min16float> coarser;

RWTexture3D<min16float> finer;

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	const min16float c000 = coarser[uint3(0, 0, 0)];
	const min16float c100 = coarser[uint3(1, 0, 0)];
	const min16float c010 = coarser[uint3(0, 1, 0)];
	const min16float c001 = coarser[uint3(0, 0, 1)];
	const min16float c110 = coarser[uint3(1, 1, 0)];
	const min16float c011 = coarser[uint3(0, 1, 1)];
	const min16float c101 = coarser[uint3(1, 0, 1)];
	const min16float c111 = coarser[uint3(1, 1, 1)];

	const min16float fx = min16float(DTid.x);
	const min16float fy = min16float(DTid.y);
	const min16float fz = min16float(DTid.z);

	const min16float x_weight = abs(fx - 0.5f) / (abs(fx - 0.5f) + abs(fx - 2.5f));
	const min16float y_weight = abs(fy - 0.5f) / (abs(fy - 0.5f) + abs(fy - 2.5f));
	const min16float z_weight = abs(fz - 0.5f) / (abs(fz - 0.5f) + abs(fz - 2.5f));

	const min16float a = lerp(c000, c100, x_weight);
	const min16float b = lerp(c010, c110, x_weight);
	const min16float c = lerp(c001, c101, x_weight);
	const min16float d = lerp(c011, c111, x_weight);
	const min16float e = lerp(a, b, y_weight);
	const min16float f = lerp(c, d, y_weight);
	const min16float g = lerp(e, f, z_weight);

	finer[DTid.xyz] = g;
}