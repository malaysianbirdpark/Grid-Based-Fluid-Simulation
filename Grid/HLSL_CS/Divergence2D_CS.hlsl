Texture2D<float2> x_in : register(t0);

RWTexture2D<float> div : register(u0);

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

	const float2 up = x_in.SampleLevel(sampler0, pos + float2(0.0f, dh), 0.0f);
	const float2 down = x_in.SampleLevel(sampler0, pos - float2(0.0f, dh), 0.0f);
	const float2 right = x_in.SampleLevel(sampler0, pos + float2(dw, 0.0f), 0.0f);
	const float2 left = x_in.SampleLevel(sampler0, pos - float2(dw, 0.0f), 0.0f);

	div[DTid.xy] = (right.x - left.x) * dw + (up.y - down.y) * dh;
}