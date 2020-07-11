SamplerState SampleState : register(s0);

cbuffer Matrices : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
};

struct Vertex
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
 //   float2 UV : TEXCOORD;
};

struct Pixel
{
    float4 Projected : SV_POSITION;
    float3 Normal : TEXCOORD0;
//    float2 UV : TEXCOORD1;
};

Pixel VS(Vertex Input)
{
    Pixel Output = (Pixel) 0;
    
    Input.Position.w = 1.0f;
	Output.Projected = mul(Input.Position, World);
    
    Output.Projected = mul(Output.Projected, View);
    Output.Projected = mul(Output.Projected, Projection);
    
	Output.Normal = normalize(mul(Input.Normal, World));
    
  //  Output.UV = Input.UV;
    
    return Output;

}

float4 PS(Pixel Input) : SV_Target0
{
    float4 FinalColor;
    
    FinalColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    return FinalColor;
}