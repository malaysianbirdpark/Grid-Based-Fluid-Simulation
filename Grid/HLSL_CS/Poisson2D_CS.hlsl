Texture2D x_in : register(t0);
Texture2D b_in : register(t1);

RWTexture2D<float4> x_out : register(u0);

cbuffer constants : register(b2) {
	float alpha;
	float r_beta;
};

SamplerState sampler0 : register(s0);

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	x_in.GetDimensions(width, height);

	const float dw = 1.0f / width;
	const float dh = 1.0f / height;
	const float2 dr = float2(dw, dh);
	const float2 pos = DTid.xy * dr;

	const float4 up = x_in.SampleLevel(sampler0, pos + float2(0.0f, dh), 0.0f);
	const float4 down = x_in.SampleLevel(sampler0, pos - float2(0.0f, dh), 0.0f);
	const float4 right = x_in.SampleLevel(sampler0, pos + float2(dw, 0.0f), 0.0f);
	const float4 left = x_in.SampleLevel(sampler0, pos - float2(dw, 0.0f), 0.0f);

	x_out[DTid.xy] = (up + down + left + right - alpha * b_in.SampleLevel(sampler0, pos, 0.0f)) * r_beta;
}