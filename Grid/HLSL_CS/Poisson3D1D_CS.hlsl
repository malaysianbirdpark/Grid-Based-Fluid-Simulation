Texture3D<float> x_in : register(t0);
Texture3D<float> b_in : register(t1);

RWTexture3D<float> x_out : register(u0);

cbuffer constants : register(b2) {
	float alpha;
	float r_beta;
};

[numthreads(16, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	uint depth;
	x_in.GetDimensions(width, height, depth);

	if (DTid.x == 0 && DTid.y == 0 && DTid.z == 0) {
        x_out[DTid.xyz] = 0.0f;
		return;
    }

    const float up = x_in[uint3(DTid.x, min(DTid.y + 1, height - 1), DTid.z)];
    const float down = x_in[uint3(DTid.x, max(DTid.y - 1, 0), DTid.z)];
    const float right = x_in[uint3(min(DTid.x + 1, width - 1), DTid.y, DTid.z)];
    const float left = x_in[uint3(max(DTid.x - 1, 0), DTid.y, DTid.z)];
    const float front = x_in[uint3(DTid.x, DTid.y, min(DTid.z + 1, depth - 1))];
    const float behind = x_in[uint3(DTid.x , DTid.y, max(DTid.z - 1, 0))];

	x_out[DTid.xyz] = (up + down + left + right + front + behind + alpha * b_in[DTid.xyz]) * r_beta;
}