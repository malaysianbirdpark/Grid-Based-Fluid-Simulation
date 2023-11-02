Texture3D<min16float4> x_in : register(t0);

RWTexture3D<min16float> div : register(u0);

SamplerState sampler1 : register(s1);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint width;
    uint height;
    uint depth;
    x_in.GetDimensions(width, height, depth);

    const min16float3 dr = min16float3((1.0f / width), (1.0f / height), (1.0f / depth));

    const min16float right = x_in.SampleLevel(sampler1, (min16float3(DTid.x + 1, DTid.y, DTid.z) + 0.5f) * dr, 0.0f).x;
    const min16float left = x_in.SampleLevel(sampler1, (min16float3(DTid.x - 1, DTid.y, DTid.z) + 0.5f) * dr, 0.0f).x;

    const min16float up = x_in.SampleLevel(sampler1, (min16float3(DTid.x, DTid.y + 1, DTid.z) + 0.5f) * dr, 0.0f).y;
    const min16float down = x_in.SampleLevel(sampler1, (min16float3(DTid.x, DTid.y - 1, DTid.z) + 0.5f) * dr, 0.0f).y;

    const min16float front = x_in.SampleLevel(sampler1, (min16float3(DTid.x, DTid.y, DTid.z + 1) + 0.5f) * dr, 0.0f).z;
    const min16float behind = x_in.SampleLevel(sampler1, (min16float3(DTid.x, DTid.y, DTid.z - 1) + 0.5f) * dr, 0.0f).z;

	//const min16float right  = DTid.x + 1 < width - 1  ? x_in[uint3(DTid.x + 1, DTid.y, DTid.z)].x : 0.0f;
	//const min16float left   = DTid.x - 1 > 0          ? x_in[uint3(DTid.x - 1, DTid.y, DTid.z)].x : 0.0f;

	//const min16float up     = DTid.y + 1 < height - 1 ? x_in[uint3(DTid.x, DTid.y + 1, DTid.z)].y : 0.0f;
	//const min16float down   = DTid.y - 1 > 0          ? x_in[uint3(DTid.x, DTid.y - 1, DTid.z)].y : 0.0f;

	//const min16float front  = DTid.z + 1 < depth - 1  ? x_in[uint3(DTid.x, DTid.y, DTid.z + 1)].z : 0.0f;
	//const min16float behind = DTid.z - 1 > 0          ? x_in[uint3(DTid.x, DTid.y, DTid.z - 1)].z : 0.0f;

	div[DTid.xyz] = ((right - left) + (up - down) + (front - behind)) * 0.5f;
}