Texture3D<min16float>  p : register(t0);
Texture3D<min16float3> v : register(t1);

RWTexture3D<min16float3> result     : register(u0);

SamplerState sampler1 : register(s1);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint width;
    uint height;
    uint depth;
    p.GetDimensions(width, height, depth);

    if (DTid.x > 0 && DTid.x < width - 1 && DTid.y > 0 && DTid.y < height - 1 && DTid.z > 0 && DTid.z < depth - 1)
    {
        const min16float center = p[DTid.xyz];

        min16float3 obstacle_velocity = 0.0f;
        min16float3 velocity_mask     = 1.0f;

        min16float right  = p[uint3(DTid.x + 1, DTid.y, DTid.z)];
        min16float left   = p[uint3(DTid.x - 1, DTid.y, DTid.z)];
        min16float up     = p[uint3(DTid.x, DTid.y + 1, DTid.z)];
        min16float down   = p[uint3(DTid.x, DTid.y - 1, DTid.z)];
        min16float front  = p[uint3(DTid.x, DTid.y, DTid.z + 1)];
        min16float behind = p[uint3(DTid.x, DTid.y, DTid.z - 1)];

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

        result[DTid.xyz] = (v[DTid.xyz] - min16float3(right - left, up - down, front - behind) * 0.5f) * velocity_mask + obstacle_velocity;
    }
    else {
        result[DTid.xyz] = 0.0f;
    }
}