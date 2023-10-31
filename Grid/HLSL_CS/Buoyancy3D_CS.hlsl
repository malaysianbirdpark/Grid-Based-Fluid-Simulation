Texture3D<min16float3> velocity_in : register(t0);

RWTexture3D<min16float3> velocity_out : register(u0);

cbuffer constants : register(b1) {
	min16float dt;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	velocity_out[DTid.xyz] = velocity_in[DTid.xyz];
}