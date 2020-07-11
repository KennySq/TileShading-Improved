SamplerState SampleDefault : register(s0);

Texture2D Texture1 : register(t0);
Texture2D Texture2 : register(t1);
Texture2D Texture3 : register(t2);
Texture2D Texture4 : register(t3);
Texture2D Texture5 : register(t4);
Texture2D Texture6 : register(t5);
Texture2D Texture7 : register(t6);

struct UVVertex
{
    float4 Position : POSITION0;
    float2 UV : TEXCOORD0;
};

struct Pixel
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
};

Pixel SampleVS(UVVertex Input)
{
    Pixel Output = (Pixel) 0;
    
    Output.Position = Input.Position;
    Output.UV = Input.UV;
   
    
	return Output;
}

float4 SamplePS(Pixel Input) : SV_Target0
{
    float4 Color = Input.Position;
    
    float4 WorldPosition = Texture1.Sample(SampleDefault, Input.UV);
    float4 WorldNormal = Texture2.Sample(SampleDefault, Input.UV) + 0.3f;
    
    float Depth;
    
    Depth = WorldPosition.z;
    Color = Texture7.Sample(SampleDefault, Input.UV);

    return Color;
    
}