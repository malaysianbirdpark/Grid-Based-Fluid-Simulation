struct GS_IN {
	float3 velocity : VELOCITY;
    uint   slice    : INDEX;
	float4 pos      : SV_POSITION;
};

struct GS_OUT
{
	float3 velocity : VELOCITY;
	uint   slice    : SV_RenderTargetArrayIndex;
	float4 pos      : SV_POSITION;
};

[maxvertexcount(3)]
void main(
	triangle GS_IN input[3], 
	inout TriangleStream<GS_OUT> output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;
		element.velocity = input[i].velocity;
		element.slice = input[i].slice;
		element.pos = input[i].pos;
		output.Append(element);
	}
}