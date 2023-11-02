Texture3D<min16float>  x_in       : register(t0);
Texture3D<min16float4> sub_target : register(t1);

RWTexture3D<min16float4> result     : register(u0);

SamplerState sampler1 : register(s1);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint width;
    uint height;
    uint depth;
    x_in.GetDimensions(width, height, depth);

    const min16float3 dr = min16float3((1.0f / width), (1.0f / height), (1.0f / depth));

    if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1 && DTid.z > 0 && DTid.z < depth - 1)
    {
        const min16float center = x_in[DTid.xyz];

        min16float3 obstacle_velocity = 0.0f;
        min16float3 velocity_mask     = 1.0f;

        //min16float right  = x_in[uint3(DTid.x + 1, DTid.y, DTid.z)];
        //min16float left   = x_in[uint3(DTid.x - 1, DTid.y, DTid.z)];
        //min16float up     = x_in[uint3(DTid.x, DTid.y + 1, DTid.z)];
        //min16float down   = x_in[uint3(DTid.x, DTid.y - 1, DTid.z)];
        //min16float front  = x_in[uint3(DTid.x, DTid.y, DTid.z + 1)];
        //min16float behind = x_in[uint3(DTid.x, DTid.y, DTid.z - 1)];

        min16float right = x_in.SampleLevel(sampler1, (min16float3(DTid.x + 1, DTid.y, DTid.z) + 0.5f) * dr, 0.0f);
        min16float left = x_in.SampleLevel(sampler1, (min16float3(DTid.x - 1, DTid.y, DTid.z) + 0.5f) * dr, 0.0f);

        min16float up = x_in.SampleLevel(sampler1, (min16float3(DTid.x, DTid.y + 1, DTid.z) + 0.5f) * dr, 0.0f);
        min16float down = x_in.SampleLevel(sampler1, (min16float3(DTid.x, DTid.y - 1, DTid.z) + 0.5f) * dr, 0.0f);

        min16float front = x_in.SampleLevel(sampler1, (min16float3(DTid.x, DTid.y, DTid.z + 1) + 0.5f) * dr, 0.0f);
        min16float behind = x_in.SampleLevel(sampler1, (min16float3(DTid.x, DTid.y, DTid.z - 1) + 0.5f) * dr, 0.0f);

        if (DTid.x - 1 == 0) {
            left = center; obstacle_velocity.x = 0.0f; velocity_mask.x = 0.0f;
        }
        if (DTid.x + 1 == width - 1) {
            right = center; obstacle_velocity.x = 0.0f; velocity_mask.x = 0.0f;
        }
        if (DTid.y - 1 == 0) {
            down = center; obstacle_velocity.y = 0.0f; velocity_mask.y = 0.0f;
        }
        if (DTid.y + 1 == height - 1) {
            up = center; obstacle_velocity.y = 0.0f; velocity_mask.y = 0.0f;
        }
        if (DTid.z - 1 == 0) {
            behind = center; obstacle_velocity.z = 0.0f; velocity_mask.z = 0.0f;
        }
        if (DTid.z + 1 == depth - 1) {
            front = center; obstacle_velocity.z = 0.0f; velocity_mask.z = 0.0f;
        }

        result[DTid.xyz] = min16float4((sub_target[DTid.xyz].xyz - min16float3(right - left, up - down, front - behind) * 0.5f) * velocity_mask + obstacle_velocity, sub_target[DTid.xyz].w);
    }
    else {
        result[DTid.xyz] = min16float4(0.0f, 0.0f, 0.0f, sub_target[DTid.xyz].w);
    }
}