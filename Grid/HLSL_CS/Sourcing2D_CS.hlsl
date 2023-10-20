RWTexture2D<float2> velocity_in : register(u0);
RWTexture2D<float4> quantity_in : register(u1);

RWTexture2D<float2> velocity : register(u2);
RWTexture2D<float4> quantity : register(u3);

cbuffer color : register(b0) {
	float4 fluid_color;
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	quantity_in.GetDimensions(width, height);

	velocity[DTid.xy] = velocity_in[DTid.xy];
    //quantity[DTid.xy] = quantity_in[DTid.xy] - 0.0000001f;
    quantity[DTid.xy] = quantity_in[DTid.xy];

	// two squares
	//if (DTid.x >= 100.0f && DTid.x <= 150.0f && DTid.y >= 200.0f && DTid.y <= 250.0f) {
	//	velocity[DTid.xy] += float2(0.00001f, 0.0f);
	//	quantity[DTid.xy] += fluid_color * 0.27f;
	//}

	//if (DTid.x <= 500.0f && DTid.x >= 450.0f && DTid.y >= 200.0f && DTid.y <= 250.0f) {
	//	velocity[DTid.xy] -= float2(0.001155f, 0.0f);
	//	quantity[DTid.xy] += fluid_color * 0.27f;
	//}

	// circle at the center

	if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1) {
        velocity[DTid.xy] += normalize(float2(0.0f, -1.0f)) * 0.0011f;
        if ((DTid.x - 320) * (DTid.x - 320) + (DTid.y - 320) * (DTid.y - 320) <= 1600) {
            quantity[DTid.xy] += fluid_color * 0.047f;
        }
	}

    //if ((DTid.x - 320) * (DTid.x - 320) + (DTid.y - 320) * (DTid.y - 320) > 1600)
    //    if ((DTid.x - 320) * (DTid.x - 320) + (DTid.y - 320) * (DTid.y - 320) <= 10000)
    //        velocity[DTid.xy] += normalize(float2(DTid.x - 320, DTid.y - 320)) * 0.0001f;
            //velocity[DTid.xy] += normalize(float2(400 - 320, 200 - 320)) * 0.0001f;
}