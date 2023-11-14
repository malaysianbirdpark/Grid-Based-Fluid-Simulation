struct PS_IN {
    float3 world_pos : POSITION;
    float3 uvw       : TEXCOORD;
};

struct PS_OUT {
    min16float3 uvw   : SV_TARGET0;
    min16float3 world : SV_TARGET1;
};

PS_OUT main(PS_IN input)
{
    PS_OUT output;
    output.uvw = input.uvw;
    output.world = input.world_pos;
    return output;
}