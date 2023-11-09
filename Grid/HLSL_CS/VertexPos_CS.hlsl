RWBuffer<float4> vertices : register(u0);

RWBuffer<float4> prev_pos : register(u1);

cbuffer mvp : register(b2) 
{
    matrix m;
    matrix mit;
    matrix mvp;
};

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    prev_pos[DTid.x] = mul(vertices[DTid.x], m);
}