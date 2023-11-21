Texture3D<min16float3> velocity_in : register(t0);
Texture3D<min16float2> quantity_in : register(t1);

Texture2DArray<uint> obstacle : register(t10);

RWTexture3D<min16float3> velocity : register(u0);
RWTexture3D<min16float2> quantity : register(u1);

cbuffer color : register(b0) {
	min16float3 dir;
	min16float  speed;
	min16float  quantity_scale;
	min16float  dissipation;
}

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint width;
	uint height;
	uint depth;
	velocity_in.GetDimensions(width, height, depth);

	velocity[DTid.xyz] = velocity_in[DTid.xyz];
    quantity[DTid.xyz] = quantity_in[DTid.xyz];

    if (obstacle[DTid.xyz].r <= 0.9f) {
		if (DTid.y > height - 50 && DTid.y <= height - 2)
		{
			velocity[DTid.xyz] = 0.0f;
			quantity[DTid.xyz] += min16float2(100.0f, 0.0f) * speed;
		}
    }
}
