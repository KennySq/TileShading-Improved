cbuffer ConstantBufferVariables : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};

struct Vertex
{
    float4 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 UV : TEXCOORD0;
};

struct GBuffer
{
    float4 Position : SV_POSITION0;
    float4 WorldPosition : TEXCOORD0;
    
    float4 Normal : TEXCOORD1;
    float2 UV : TEXCOORD2;
    float4 ViewPosition : TEXCOORD3;
    
};

struct Pixel
{
    float4 WorldPosition : SV_Target0;
    float4 WorldNormal : SV_Target1;
    float2 UV : SV_Target2;
    float4 ViewPosition : SV_Target3;
    
};

bool IsInit = false;

GBuffer DrawBufferVS(Vertex Input)
{
    GBuffer Output = (GBuffer) 0;
    
    float4 WorldPos;
    Input.Position.w = 1.0f;
    WorldPos = mul(Input.Position, World);
    Output.WorldPosition = WorldPos;
    Output.Position = mul(WorldPos, View);
    Output.ViewPosition = Output.Position;

    Output.Position = mul(Output.Position, Projection);

    Output.Normal = mul(float4(Input.Normal, 1.0f), World);
    
    Output.UV = Input.UV;
    
    return Output;
}



Pixel DrawBufferPS(GBuffer Input)
{
    Pixel Output = (Pixel) 0;
    
    Output.WorldPosition = Input.WorldPosition;
    Output.WorldNormal = normalize(Input.Normal);
    Output.UV = Input.UV;
    Output.ViewPosition = Input.ViewPosition;
    
    return Output;
    
}

