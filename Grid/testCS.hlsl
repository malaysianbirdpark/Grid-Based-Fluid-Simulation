RWTexture2D<float4> output_texture : register(u0);

[numthreads(256, 1, 1)]
void main(uint3 gID : SV_GroupID, uint3 tID : SV_DispatchThreadID)
{
    //if (gID.x % uint(2) == 0) {
    //    output_texture[tID.xy] = float4(0.5f, 0.5f, 0.5f, 1.0f);
    //}
    //else {
    //    output_texture[tID.xy] = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //}
    output_texture[tID.xy] = float4(1.0f, 0.0f, 1.0f, 1.0f);
}