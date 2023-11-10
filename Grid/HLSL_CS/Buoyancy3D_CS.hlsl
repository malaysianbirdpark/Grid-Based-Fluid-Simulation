Texture3D<min16float3> velocity_in : register(t0);
Texture3D<min16float2> quantity_in : register(t1);

Texture2DArray<uint> obstacle : register(t6);

RWTexture3D<min16float3> velocity_out : register(u0);

cbuffer constants : register(b1) {
	min16float dt;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    velocity_in.GetDimensions(width, height, depth);

	velocity_out[DTid.xyz] = velocity_in[DTid.xyz];

	// magic number 288K for room temperature
    const min16float2 q = quantity_in[DTid.xyz];
    //if (q.r > 1e-3)
    {
        if (obstacle[DTid.xyz].r <= 0.9f)
        {
            const min16float alpha = 0.0001f;
            const min16float beta = 0.0355f;
            const min16float3 f_b = (alpha * -q.r + beta * (q.g - 288.0f)) * min16float3(0.0f, -1.0f, 0.0f);
            velocity_out[DTid.xyz] += f_b * dt;
        }
    }
}