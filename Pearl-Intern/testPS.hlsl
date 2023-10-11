struct PS_IN {
    float4 world_pos : POSITION;
};

struct PS_OUT {
    float4 color : SV_TARGET;
};

PS_OUT main(PS_IN input)
{
    PS_OUT output;

	output.color = float4(1.0f, 0.0f, 1.0f, 1.0f);

    return output;
}