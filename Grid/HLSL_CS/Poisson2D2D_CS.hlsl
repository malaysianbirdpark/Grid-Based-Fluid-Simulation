Texture2D<float2> x_in : register(s0);
Texture2D<float2> b_in : register(s1);

RWTexture2D<float2> x_out : register(u0);

cbuffer constants : register(b3) {
	float alpha;
	float r_beta;
	float viscosity;
	float dt;
};

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint width;
	uint height;
	x_in.GetDimensions(width, height);

	const float2 up = x_in[uint2(DTid.x, min(DTid.y + 1, height - 1))];
	const float2 down = x_in[uint2(DTid.x, max(DTid.y - 1, 0))];
	const float2 right = x_in[uint2(min(DTid.x + 1, width - 1), DTid.y)];
	const float2 left = x_in[uint2(max(DTid.x - 1, 0), DTid.y)];

	x_out[DTid.xy] = (up + down + right + left + alpha * b_in[DTid.xy]) * r_beta;
}