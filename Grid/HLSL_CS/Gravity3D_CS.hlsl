Texture3D<min16float3> velocity_in : register(t0);

RWTexture3D<min16float3> velocity_out : register(u0);

cbuffer constants : register(b1) {
	min16float dt;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	if (DTid.x > 1 && DTid.x < 126 && DTid.y > 1 && DTid.y < 126 && DTid.z > 1 && DTid.z < 126)
		velocity_out[DTid.xyz] = velocity_in[DTid.xyz] + min16float3(0.0f, 0.8f, 0.0f) * dt;
}