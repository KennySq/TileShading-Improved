cbuffer ConstSettings : register(b0)
{
    float4 CameraPosition;
    float4 CameraDirection;

    uint4 BWHP;
    
};

struct Ray
{
    float4 Origin;
    float4 Direction;
};

struct Triangle
{
    float4 WorldPosition[3];
};

StructuredBuffer<Triangle> TriangleBuffer : register(t1);
RWTexture2D<float4> ResultColor : register(u0);

bool IntersectTriangle(float4 RayOrigin, float4 RayDirection, float4 P0, float4 P1, float4 P2, inout float3 IntersectPoint)
{
    bool Intersected = false;
    
    float4 Edge1, Edge2;
    float3 H, S, Q;
    float A, F, U, V;
    Edge1 = P1 - P0;
    Edge2 = P2 - P0;
    
    H = cross(RayDirection.xyz, Edge2.xyz);
    A = dot(Edge1.xyz, H);
    if (A > -0.00001f && A < 0.00001f)
        return false;
    
    F = 1.0f / A;
    
    S = (RayOrigin - P0).xyz;
    U = F * dot(S, H);
    if (U < 0.0f || U > 1.0f)
        return false;
    
    Q = cross(S, Edge1.xyz);
    V = F * dot(RayDirection.xyz, Q);
    
    float t = F * dot(Edge2.xyz, Q);
    
    if (t > 0.00001f)
    {
        
        IntersectPoint = (RayOrigin + RayDirection * t).xyz;
        
        return true;
    }
    
    return false;
}