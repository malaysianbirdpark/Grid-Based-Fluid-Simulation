float3 main(float4 world_pos : POSITION) : SV_TARGET
{
	return world_pos.xyz;
}