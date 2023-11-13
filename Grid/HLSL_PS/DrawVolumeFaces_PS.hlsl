struct PS_IN {
    float4 world_pos : POSITION;
    float3 uvw       : TEXCOORD;
};

struct PS_OUT {
    float3 uvw   : SV_TARGET0;
    float3 world : SV_TARGET1;
};

PS_OUT main(PS_IN input)
{
    PS_OUT output;
    output.uvw = input.uvw;
    output.world = input.world_pos;
    return output;
}