Texture2D<float2> x_in : register(t0);
Texture2D<float2> b_in : register(t1);

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

	if (DTid.x == 0 && DTid.y == 0) {
        x_out[DTid.xy] = 0.0f;
		return;
    }

	const float2 up = x_in[uint2(DTid.x, min(DTid.y + 1, height - 1))].xy;
	const float2 down = x_in[uint2(DTid.x, max(DTid.y - 1, 0))].xy;
	const float2 right = x_in[uint2(min(DTid.x + 1, width - 1), DTid.y)].xy;
	const float2 left = x_in[uint2(max(DTid.x - 1, 0), DTid.y)].xy;

	x_out[DTid.xy].xy = (up + down + right + left + alpha * b_in[DTid.xy].xy) * r_beta;
}