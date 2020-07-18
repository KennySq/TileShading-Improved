cbuffer ConstVariables : register(b0)
{
    matrix View;
    matrix Projection;
    
};

StructuredBuffer<float4> LightPositions : register(t0);
Texture2D ProjectionBuffer : register(t1);
RWTexture2D<float> ShadowMap : register(u0);

#define MAXCASCADE 16

SamplerState DefaultSampler : register(s0);

static const float PI = 3.141592;

static const float Near = 0.01f;
static const float Far = 1000.0f;

void DevideFrustum(int Cascade, int Res)

[numthreads(16, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint Gi : SV_GroupIndex )
{
    uint Width, Height, LODs;
    ProjectionBuffer.GetDimensions(0, Width, Height, LODs);
    
    float2 Texcoord = ((float2) DTid.xy + 0.5f) / float2(Width, Height);
    float4 Projected = ProjectionBuffer.SampleLevel(DefaultSampler, Texcoord, 0.0f);
    float Depth = Projected.z / Projected.w;
    
    float FOV = 2.0f * atan(1.0f / Projection._11) * 180.0f / PI;
    
    
    
}