struct PS_IN {
    float4 world_pos  : POSITION;
    float3 normal     : NORMAL;
    float3 tangent    : TANGENT;
    float3 binormal   : BINORMAL;
    float2 uv         : TEXCOORD;
};

struct PS_OUT {
    float4 color : SV_TARGET;
};

Texture2D albedo_map   : register(t0);
Texture2D normal_map   : register(t1);
Texture2D previous     : register(t2);

SamplerState sampler2 : register(s2);

cbuffer Camera : register(b0)
{
    float3 cam_pos;
}

cbuffer DirLight : register(b1)
{
    float3 dl_dir;
    float3 dl_color;
    float  dl_intensity;
}

float4 PhongAmbient(float3 normal)
{
    static const float3 amb_down_color = float3(0.03f, 0.03f, 0.03f);
    static const float3 amb_up_color = float3(0.1f, 0.1f, 0.1f);
    const float up = normal.y * 0.5f + 0.5f;
    const float3 ambient = amb_down_color + up * amb_up_color;
    return float4(ambient, 1.0f);
}

float4 PhongDiffuse(float3 to_light, float3 normal)
{
    const float NDotL = dot(to_light, normal);
    return max(0.0f, NDotL);
}

float4 PhongSpec(const float3 to_camera, const float3 normal, const float3 to_light, const float shininess)
{
    const float3 r = reflect(-to_light, normal);
    const float NDotR = saturate(dot(r, to_camera));
    return pow(NDotR, shininess);
}

PS_OUT main(PS_IN input)
{
    PS_OUT output;

    const float4 diffuse_color = albedo_map.Sample(sampler2, input.uv);

    clip(diffuse_color.a < 0.1f ? -1 : 1);

    float3 sampled_normal = normal_map.Sample(sampler2, input.uv).xyz;
    sampled_normal = sampled_normal * 2.0f - float3(1.0f, 1.0f, 1.0f);
    const float3 binormal = cross(input.normal, input.tangent);
    const float3x3 tbn = float3x3(input.tangent, binormal, input.normal);
    const float3 normal = normalize(mul(sampled_normal, tbn));

    const float3 to_camera = normalize(cam_pos.xyz - input.world_pos);
    const float3 to_light = normalize(-dl_dir);

    // ambient
    output.color = PhongAmbient(normal) * diffuse_color;

    // diffuse
    output.color += PhongDiffuse(to_light, normal) * float4(dl_color, 1.0f) * diffuse_color * dl_intensity;

    // specular
    static const float shininess = 100.0f;
    static const float4 specular_color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.color += PhongSpec(to_camera, normal, to_light, shininess) * float4(dl_color, 1.0f) * specular_color;

    output.color = clamp(output.color, 0.0f, 0.995f);

    return output;
}