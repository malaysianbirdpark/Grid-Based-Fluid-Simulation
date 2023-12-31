Texture2D<float4> x_in : register(t0);
Texture2D<float4> b_in : register(t1);

RWTexture2D<float4> x_out : register(u0);

cbuffer constants : register(b3) {
	float alpha;
	float r_beta;
};

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	x_in.GetDimensions(width, height);

	if (DTid.x == 0 && DTid.y == 0) {
        x_out[DTid.xy] = 0.0f;
		return;
    }

    const float4 up = x_in[uint2(DTid.x, min(DTid.y + 1, height - 1))];
    const float4 down = x_in[uint2(DTid.x, max(DTid.y - 1, 0))];
    const float4 right = x_in[uint2(min(DTid.x + 1, width - 1), DTid.y)];
    const float4 left = x_in[uint2(max(DTid.x - 1, 0), DTid.y)];

	x_out[DTid.xy] = (up + down + left + right + alpha * b_in[DTid.xy]) * r_beta;
}