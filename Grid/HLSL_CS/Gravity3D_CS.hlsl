Texture3D<min16float4> velocity_in : register(t0);

RWTexture3D<min16float4> velocity_out : register(u0);

cbuffer constants : register(b1) {
	min16float dt;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (DTid.y < 120 && velocity_in[DTid.xyz].w > 1e-5)
        velocity_out[DTid.xyz] = min16float4(velocity_in[DTid.xyz] + min16float3(0.0f, 1.8f, 0.0f) * dt, velocity_in[DTid.xyz].w);
    else if (DTid.y >= 125)
        velocity_out[DTid.xyz] = min16float4(velocity_in[DTid.xyz].x, 0.0f, velocity_in[DTid.xyz].z, velocity_in[DTid.xyz].w);
    //velocity_out[DTid.xyz] = velocity_in[DTid.xyz];
}