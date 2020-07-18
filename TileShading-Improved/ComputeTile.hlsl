

struct LightProperties
{
    float4 Position;
    float4 Forward;
    float4 Color;
};

cbuffer Matrices : register(b0)
{
    matrix View;
    matrix Projection;

};

cbuffer TileSettings : register(b1)
{
    bool VisualizeTile;
    float3 Padding;
}

static const float Near = 0.1f;
static const float Far = 1000.0f;
static const uint MAXLIGHT = 64;

RWTexture2D<float4> Tile : register(u0);
RWTexture3D<uint2> Light : register(u1);

Texture2D<float4> WorldPosition : register(t0);
Texture2D<float4> NormalBuffer : register(t1);
Texture2D<float4> ViewPositionBuffer : register(t2);
StructuredBuffer<LightProperties> LightsBuffer : register(t3);

groupshared uint LightIndices[256];
groupshared uint zMin;
groupshared uint zMax;

groupshared uint LightCountPerGroup = 0;

SamplerState SampleDefault : register(s0);

void EvaluatePointLightBRDF(float4 LightDir, float4 Normal, float Intensity,float Distance, float Radius, float4 Color, inout float4 Diffuse)
{
    float4 NdotL = dot(LightDir, Normal) * Radius;
    float Attenuation = Distance;
    Color *= Intensity;
    Color /= Attenuation;
    
    Diffuse += saturate(NdotL * Color);
}
[numthreads(16, 16, 1)]
void ComputeTileCS( uint3 DTid : SV_DispatchThreadID, uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID, uint Gi : SV_GroupIndex )
{
    uint Width, Height;
    uint LightCount, LightStride;
    
    uint GroupIndex = GTid.y * 16 + GTid.x;
    
    uint SqGroupIndex = GroupIndex * GroupIndex;
    
    
    
    ViewPositionBuffer.GetDimensions(Width, Height);
    LightsBuffer.GetDimensions(LightCount, LightStride);

            
    float2 Resolution = float2((float) Width, (float) Height);
    float2 uv = float2(DTid.xy + 0.5) / Resolution;
    float4 ViewPosition = ViewPositionBuffer.SampleLevel(SampleDefault, uv, 0.0f);
    float ObjectDepth = ViewPosition.z;
    
    float Min = Far;
    float Max = Near;
    
    bool IsValid = (ObjectDepth >= Near) && (ObjectDepth < Far);
    [flatten] if(IsValid)
    {
        Min = min(Min, ObjectDepth);
        Max = max(Max, ObjectDepth);
    }
    
    if (GroupIndex == 0)
    {
        LightCountPerGroup = 0;
        zMin = 0x7F7FFFFF;
        zMax = 0;

    }
    
    GroupMemoryBarrierWithGroupSync();
    
    if(Max >= Min)
    {
        InterlockedMin(zMin, asuint(Min));
        InterlockedMax(zMax, asuint(Max));

    }
    
    GroupMemoryBarrierWithGroupSync();
    
    float TileMinZ = asfloat(zMin);
    float TileMaxZ = asfloat(zMax);
    
    
    float2 TileScale = Resolution * rcp((float) (16 * 2));
    float2 ScaleBias = TileScale - float2(Gid.xy);
    
    float4 Right = float4(Projection._11 * TileScale.x, 0.0f, ScaleBias.x, 0.0f);
    float4 Up = float4(0.0f, -Projection._22 * TileScale.y, ScaleBias.y, 0.0f);
    float4 Z = float4(0.0f, 0.0f, 1.0f, 0.0f);
    
    float4 Planes[6];
    
    Planes[0] = Z - Right;
    Planes[1] = Z + Right;
    Planes[2] = Z - Up;
    Planes[3] = Z + Up;
    
    Planes[4] = float4(0.0f, 0.0f, 1.0f, -TileMinZ);
    Planes[5] = float4(0.0f, 0.0f, -1.0f, TileMaxZ);
    
    GroupMemoryBarrierWithGroupSync();
    
    // 절두체 평면 정규화 (Near, Far 요소는 이미 정규화되어 있으므로 제외)
    [unroll] for (uint i = 0; i < 4; i++)
        Planes[i] *= rcp(length(Planes[i]));

    
    for (uint LightIndex = GroupIndex; LightIndex < LightCount; LightIndex+=256)
    {
        float4 SurfaceWorld = WorldPosition.SampleLevel(SampleDefault, uv, 0.0f);
        
        float4 LightViewPosition = LightsBuffer[LightIndex].Forward;
        
        float Radius = LightsBuffer[LightIndex].Color.w;
        float4 LightWorldPosition = LightsBuffer[LightIndex].Position;
        float Distance = distance(SurfaceWorld, LightWorldPosition);
        bool IsInside = true;
        [unroll] for (uint i = 0; i < 6; i++)
        {
            LightWorldPosition.w = 1.0f;
            LightViewPosition = mul(LightWorldPosition, View);
            
            float Perp = dot(Planes[i], float4(LightViewPosition.xyz, 1.0f));
            IsInside = IsInside && (Perp >= -Radius);
        }

        [branch] if (IsInside)
        {
            uint ListIndex;
            InterlockedAdd(LightCountPerGroup, 1, ListIndex);
            LightIndices[ListIndex] = LightIndex;

        }

    }
    
    GroupMemoryBarrierWithGroupSync();

    Tile[DTid.xy] = float4(0.0f, 0.0f, 0.0f, 0.0f);
    uint LCount = LightCountPerGroup;
    GroupMemoryBarrierWithGroupSync();
    float4 WorldPos = WorldPosition.SampleLevel(SampleDefault, uv, 0.0f);
    if (all(DTid.xy < Resolution))
    {
        float4 Color = (float(LCount)).xxxx; //dot(LightsBuffer[;
        [branch]
        if (LCount >= 4) 
            Color.gb = 0;
        
       //   Tile[DTid.xy] = LCount/4.0f;
        float4 Diffuse = 0.0f;
        for (uint j = 0; j < LCount; j++)
        {
            LightProperties Light = LightsBuffer[LightIndices[j]];
               
            float4 Normal = NormalBuffer.SampleLevel(SampleDefault, uv, 0.0f);
            float4 Direction = normalize(Light.Position - WorldPos);
            float Radius = Light.Color.w;
            float Intensity = Light.Position.w;
            float Dist = distance(WorldPos, Light.Position);
            float Atten = Dist * rcp(Dist);

            EvaluatePointLightBRDF(Direction, Normal, Intensity,Dist, Radius, Light.Color, Diffuse);
        }
        
        GroupMemoryBarrierWithGroupSync();
        
        if(!VisualizeTile)
            Tile[DTid.xy] = saturate(Diffuse.xyzw);
        else
            Tile[DTid.xy] = LCount/255.0f;// Diffuse.xxxx; //(float) LCount; //Diffuse.xxxx;
        
        
    }

    GroupMemoryBarrierWithGroupSync();
        
}