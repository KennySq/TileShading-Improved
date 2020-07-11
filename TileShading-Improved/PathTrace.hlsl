//#define EPSILON 0.00001f

struct Ray
{

    float4 Origin : POSITION0;
    float4 Direction : NORMAL0;
   
};

struct Vertex
{
    float4 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 UV : TEXCOORD0;
   
};

cbuffer ConstantBufferVariables : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

};
cbuffer Global : register(b1)
{
    uint Bounce;
    float4 CameraPosition;
    
    uint Width;
    uint Height;
    
    bool IsBufferInitialized;
};
SamplerState DefaultSampler : register(s0);



struct Hit
{
    float4 HitPosition : SV_Position;
    float4 Direction : TEXCOORD0;
};

struct GSInput
{
    float4 Position : SV_Position;
    float4 WorldPosition : TEXCOORD0;
    float4 Normal : TEXCOORD1;
    float2 UV : TEXCOORD2;
};



GSInput PathVS(Vertex Input)
{
    GSInput Output = (GSInput) 0;
    
    float4 WorldPos;
    float4 WorldNormal;
    float4 Path;
    
    WorldPos = mul(Input.Position, World);
    Output.Position = mul(WorldPos, View);
    Output.Position = mul(Output.Position, Projection);
    Output.WorldPosition = WorldPos;
    Output.Normal = mul(float4(Input.Normal, 1.0f), World);
    Output.UV = Input.UV;
    
    return Output;
}

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

[maxvertexcount(4)]
void PathGS(triangle GSInput Input[3],
	inout PointStream<Hit> Output)
{
    float4 UVPosition;
    
    // Viewport ∫Ò¿≤
    float4 Horizontal = float4(2.0f, 0.0f, 0.0f, 0.0f);
    float4 Vertical = float4(0.0f, 2.0f, 0.0f, 0.0f);
    
    for (uint i = 0; i < Height; i++)
    {
        for (uint j = 0; j < Width; j++)
        {
            
            Hit HitRecord;
            
            double U = (double) (j) / (Width - 1);
            double V = (double) (i) / (Height - 1);
            
            float3 Intersection = 0.0f;
            
            UVPosition = (U * Horizontal + V * Vertical);
            
            Ray R;
            R.Origin = CameraPosition;
            R.Direction = UVPosition;

            
            
            bool IntT = IntersectTriangle(R.Origin, R.Direction, Input[0].WorldPosition,
            Input[1].WorldPosition, Input[2].WorldPosition, Intersection);
               
            if (IntT == false)
                continue;
            
            float3 SurfaceNormal;
            
            SurfaceNormal = normalize(cross((Input[1].Position - Input[0].Position).xyz, (Input[2].Position - Input[0].Position).xyz));
            
            HitRecord.HitPosition = float4(Intersection, 1.0f);
            HitRecord.Direction = float4(normalize(reflect(R.Direction.xyz, SurfaceNormal)), 1.0f);
            
            Output.Append(HitRecord);
            
        }

    }
}

float4 PathPS(Hit Input) : SV_Target0
{
    float4 Color;
    
    Color = Input.HitPosition;
    
    return Color;
}

//[maxvertexcount(4)]
//void PathGS(point Hit Input[1],
//	inout PointStream<Hit> Output)
//{
//    Hit element;
//    Vertex R = (Vertex)0;
//    //RayPositionBuffer.Sample(DefaultSampler, );
    
//    element.HitPosition = Input[0].HitPosition;
//    element.Direction = normalize(Input[0].Direction);
//    Output.Append(element);
    
//}