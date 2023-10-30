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

    if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1 && DTid.z > 0 && DTid.z < depth - 1)
    {
        const unorm float4 right  = x_in[uint3(DTid.x + 1, DTid.y, DTid.z)];
        const unorm float4 left   = x_in[uint3(DTid.x - 1, DTid.y, DTid.z)];
        const unorm float4 up     = x_in[uint3(DTid.x, DTid.y + 1, DTid.z)];
        const unorm float4 down   = x_in[uint3(DTid.x, DTid.y - 1, DTid.z)];
        const unorm float4 front  = x_in[uint3(DTid.x, DTid.y, DTid.z + 1)];
        const unorm float4 behind = x_in[uint3(DTid.x, DTid.y, DTid.z - 1)];

		x_out[DTid.xyz] = (unorm float4)(((x_in[DTid.xyz] + (up + down + left + right + front + behind) * alpha) * r_beta).xyz, x_in[DTid.xyz].a);
    }
}
