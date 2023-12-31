Texture3D<min16float>  p : register(t0);
Texture3D<min16float4> v : register(t1);

Texture2DArray<uint>        obstacle : register(t10);
Texture2DArray<min16float3> obstacle_vel : register(t11);

RWTexture3D<min16float4> result     : register(u0);

SamplerState sampler1 : register(s1);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (obstacle[DTid.xyz].r <= 0.9f)
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

        if (obstacle[uint3(DTid.x - 1, DTid.y, DTid.z)].r > 0.9f) {
            left = center; obstacle_velocity.x = obstacle_vel[uint3(DTid.x - 1, DTid.y, DTid.z)].x; velocity_mask.x = 0.0f;
        }
        if (obstacle[uint3(DTid.x + 1, DTid.y, DTid.z)].r > 0.9f) {
            right = center; obstacle_velocity.x = obstacle_vel[uint3(DTid.x + 1, DTid.y, DTid.z)].x; velocity_mask.x = 0.0f;
        }
        if (obstacle[uint3(DTid.x, DTid.y - 1, DTid.z)].r > 0.9f) {
            down = center; obstacle_velocity.y = obstacle_vel[uint3(DTid.x, DTid.y - 1, DTid.z)].y; velocity_mask.y = 0.0f;
        }
        if (obstacle[uint3(DTid.x, DTid.y + 1, DTid.z)].r > 0.9f) {
            up = center; obstacle_velocity.y = obstacle_vel[uint3(DTid.x, DTid.y + 1, DTid.z)].y; velocity_mask.y = 0.0f;
        }
        if (obstacle[uint3(DTid.x, DTid.y, DTid.z - 1)].r > 0.9f) {
            behind = center; obstacle_velocity.z = obstacle_vel[uint3(DTid.x, DTid.y, DTid.z - 1)].z; velocity_mask.z = 0.0f;
        }
        if (obstacle[uint3(DTid.x, DTid.y, DTid.z + 1)].r > 0.9f) {
            front = center; obstacle_velocity.z = obstacle_vel[uint3(DTid.x, DTid.y, DTid.z + 1)].z; velocity_mask.z = 0.0f;
        }

        const min16float div = v[DTid.xyz].w;
        result[DTid.xyz] = min16float4((v[DTid.xyz].xyz - min16float3(right - left, up - down, front - behind) * 0.5f) * velocity_mask + obstacle_velocity, 0.0f);
    }
    else {
        result[DTid.xyz] = min16float4(obstacle_vel[DTid.xyz], 0.0f);
    }
}