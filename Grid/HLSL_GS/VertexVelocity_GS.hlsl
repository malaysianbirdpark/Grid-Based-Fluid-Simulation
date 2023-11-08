struct GS_IN
{
    float4 pos       : SV_POSITION;
    float3 velocity  : VELOCITY;
};

struct GS_OUT {
    float3 velocity    : VELOCITY;
    uint   slice_index : SV_RenderTargetArrayIndex;
    float4 pos         : SV_POSITION;
};

struct GS_INT_OUT {
    float3 velocity    : VELOCITY;
    uint   slice_index : SV_RenderTargetArrayIndex;
    float2 pos         : POSITION;
};

cbuffer slice_info : register(b0) 
{
    uint  slice_index;
    float slice_depth;
    uint  width;
    uint  height;
}

void GetEdgePlaneIntersection(GS_IN vA, GS_IN vB, inout GS_INT_OUT intersections[2], inout int idx);

[maxvertexcount(4)]
void main(
    triangle GS_IN input[3], 
    inout TriangleStream<GS_OUT> triStream)
{
    GS_OUT output;
    output.slice_index = slice_index;
    float min_z = min(min(input[0].pos.z, input[1].pos.z), input[2].pos.z);
    float max_z = max(max(input[0].pos.z, input[1].pos.z), input[2].pos.z);
    if ((slice_depth < min_z) || (slice_depth > max_z))     // This triangle doesn't intersect the slice.    
        return;

    GS_INT_OUT intersections[2];
    for (int i = 0; i < 2; i++)
    {
        intersections[i].pos = 0;
        intersections[i].velocity = 0;
    }

    int idx = 0;
    if (idx < 2)
        GetEdgePlaneIntersection(input[0], input[1], intersections, idx);
    if (idx < 2)
        GetEdgePlaneIntersection(input[1], input[2], intersections, idx);
    if (idx < 2)
        GetEdgePlaneIntersection(input[2], input[0], intersections, idx);
    if (idx < 2)
        return;

    float sqrt2 = 1.414; // The diagonal of a pixel    
    float2 normal = sqrt2 * normalize(cross((input[1].pos - input[0].pos), (input[2].pos - input[0].pos)).xy);
    static const float2 dimensions = float2((1.0f / width), (1.0f / height));
    for (int i = 0; i < 2; i++)
    {
        output.pos = float4(intersections[i].pos, 0, 1);
        output.velocity = intersections[i].velocity;
        triStream.Append(output);
        output.pos = float4((intersections[i].pos + (normal * dimensions)), 0, 1);
        output.velocity = intersections[i].velocity;
        triStream.Append(output);
    }

    triStream.RestartStrip();
}

void GetEdgePlaneIntersection(GS_IN vA, GS_IN vB, inout GS_INT_OUT intersections[2], inout int idx)
{
    float t = (slice_depth - vA.pos.z) / (vB.pos.z - vA.pos.z);
    if ((t < 0) || (t > 1))
        // Line-plane intersection is not within the edge's end points    
        // (A and B)    
        return;

    intersections[idx].pos = lerp(vA.pos, vB.pos, t).xy;
    intersections[idx].velocity = lerp(vA.velocity, vB.velocity, t);
    idx++;
}