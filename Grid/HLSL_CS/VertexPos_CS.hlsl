Texture1D<float3> vertices : register(t0);

RWTexture1D<float3> prev_pos : register(u0);
RWTexture1D<float3> cur_pos : register(u1);

cbuffer mvp : register (b0) 
{
    matrix m;
    matrix mit;
    matrix mvp;
};

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    prev_pos[DTid.x] = cur_pos[DTid.x];
    cur_pos[DTid.x] = mul(float4(vertices[DTid.x], 1.0f), m);
}