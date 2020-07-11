cbuffer ConstSettings : register(b1)
{

    float4 CameraPosition;
    float4 CameraDirection;

    float4 BWHP;
    float4 Padding;

    
    //uint Bounces;
    //uint Width;
    //uint Height;
    //uint Padding;
};

cbuffer Matrices : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

struct Ray
{
    float4 Origin;
    float4 Direction;
};

struct Vertex
{
    float4 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 UV : TEXCOORD0;
};

struct Triangle
{
    Vertex Vertices[3];
};

StructuredBuffer<Triangle> TriangleBuffer : register(t0);
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

[numthreads(8,8, 8)]
void TextureWriteCS( uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID, uint Gi : SV_GroupIndex )
{
    Ray InitialRays;
    ResultColor[DTid.yz] = 0.0f;
    InitialRays.Origin = CameraPosition;
    InitialRays.Direction = CameraDirection;
    
    uint TriIndex = DTid.x;
    float4 P0;
    float4 P1;
    float4 P2;
    float3 CrossPoint;
    
    P0 = TriangleBuffer[TriIndex].Vertices[0].Position;
    P1= TriangleBuffer[TriIndex].Vertices[1].Position;
    P2 = TriangleBuffer[TriIndex].Vertices[2].Position;
    
    float4 WorldP0 = mul(P0, World);
    float4 WorldP1 = mul(P1, World);
    float4 WorldP2 = mul(P2, World);
    GroupMemoryBarrierWithGroupSync();
    
    IntersectTriangle(InitialRays.Origin, InitialRays.Direction, WorldP0, WorldP1, WorldP2, CrossPoint);
    
    ResultColor[DTid.yz] = float4(CrossPoint, 1.0f);
}