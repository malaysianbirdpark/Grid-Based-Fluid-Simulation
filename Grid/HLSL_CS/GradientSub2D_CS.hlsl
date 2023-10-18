Texture2D<float> x_in : register(t0);

RWTexture2D<float2> result : register(u0);

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

	const float up = x_in.SampleLevel(sampler0, pos + float2(0.0f, dh), 0.0f);
	const float down = x_in.SampleLevel(sampler0, pos - float2(0.0f, dh), 0.0f);
	const float right = x_in.SampleLevel(sampler0, pos + float2(dw, 0.0f), 0.0f);
	const float left = x_in.SampleLevel(sampler0, pos - float2(dw, 0.0f), 0.0f);

	result[DTid.xy] = x_in.SampleLevel(sampler0, DTid.xy * dr, 0.0f) - float2(right - left, up - down) * dr;
}