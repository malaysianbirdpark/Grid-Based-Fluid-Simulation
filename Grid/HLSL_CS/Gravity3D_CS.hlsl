Texture3D<min16float4> velocity_in : register(t0);

RWTexture3D<min16float4> velocity_out : register(u0);

cbuffer constants : register(b1) {
	min16float dt;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    //if (DTid.y < 126) {
    //    const float factor = velocity_in[DTid.xyz].w * 1e-4;
    //    velocity_out[DTid.xyz] = min16float4(velocity_in[DTid.xyz] + min16float3(0.0f, 9.8f * factor, 0.0f) * dt, velocity_in[DTid.xyz].w);
    //}
    velocity_out[DTid.xyz] = velocity_in[DTid.xyz];
}