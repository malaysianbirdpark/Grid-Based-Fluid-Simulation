Texture3D<unorm float4> x_in : register(t0);

RWTexture3D<unorm float4> x_out : register(u0);

cbuffer constants : register(b2) {
	float alpha;
	float r_beta;
	float viscosity;
	float dt;
};

SamplerState sampler1 : register(s1);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint width;
	uint height;
	uint depth;
	x_in.GetDimensions(width, height, depth);

    //if (DTid.x == 0 || DTid.y == 0 || DTid.z == 0)
    //{
    //    x_out[DTid.xyz] = 0.0f;
    //    return;
    //}

    const min16float3 right = x_in[uint3(min(DTid.x + 1, width - 1), DTid.y, DTid.z)].rgb;
    const min16float3 left = x_in[uint3(max(DTid.x - 1, 0), DTid.y, DTid.z)].rgb;
    const min16float3 up = x_in[uint3(DTid.x, min(DTid.y + 1, height - 1), DTid.z)].rgb;
    const min16float3 down = x_in[uint3(DTid.x, max(DTid.y - 1, 0), DTid.z)].rgb;
    const min16float3 front = x_in[uint3(DTid.x, DTid.y, min(DTid.z + 1, depth - 1))].rgb;
    const min16float3 behind = x_in[uint3(DTid.x, DTid.y, max(DTid.z - 1, 0))].rgb;

    x_out[DTid.xyz] = float4((x_in[DTid.xyz] + (up + down + left + right + front + behind) * alpha) * r_beta, x_in[DTid.xyz].a);
}
