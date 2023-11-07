Texture2D<float2> velocity_in : register(t0);
Texture2D<float4> quantity_in : register(t1);

RWTexture2D<float2> velocity : register(u0);
RWTexture2D<float4> quantity : register(u1);

cbuffer color : register(b0) {
	float2 dir;
	float  color_scale;
	float  speed;
	float4 color;
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	quantity_in.GetDimensions(width, height);

	velocity[DTid.xy].xy = velocity_in[DTid.xy].xy;
    quantity[DTid.xy] = quantity_in[DTid.xy];

	// circle at the center
	if (DTid.x < width && DTid.y < height) {
        if ((DTid.x - (width / 2)) * (DTid.x - (width / 2)) + (DTid.y - (height / 2)) * (DTid.y - (height / 2)) <= 1600) {
            velocity[DTid.xy].xy += normalize(float2(dir.xy)) * speed;
            quantity[DTid.xy] += color * color_scale;
        }
	}
}