struct PS_IN {
    float4 world_pos : POSITION;
    float2 uv        : TEXCOORD;
};

struct PS_OUT {
    float4 color : SV_TARGET;
};

Texture2D diffuse_map : register(t0);
SamplerState sampler0 : register(s0);

PS_OUT main(PS_IN input)
{
    PS_OUT output;

    output.color = diffuse_map.Sample(sampler0, input.uv);

    return output;
}