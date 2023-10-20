RWTexture2D<float2> velocity_in : register(u0);
RWTexture2D<float4> quantity_in : register(u1);

RWTexture2D<float2> velocity : register(u2);
RWTexture2D<float4> quantity : register(u3);

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

	velocity[DTid.xy] = velocity_in[DTid.xy];
    quantity[DTid.xy] = quantity_in[DTid.xy];

	// circle at the center
	if (DTid.x >= 0 && DTid.x < width && DTid.y >= 0 && DTid.y < height) {
        velocity[DTid.xy] += normalize(float2(dir.xy)) * speed;
        if ((DTid.x - 320) * (DTid.x - 320) + (DTid.y - 320) * (DTid.y - 320) <= 1600) {
            quantity[DTid.xy] += color * color_scale;
        }
	}
}