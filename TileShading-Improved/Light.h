#pragma once
#define MAX_PIXLIGHTS 64

class Light
{

	
public:
	XMFLOAT4 Position;
	XMFLOAT4 Forward;
	XMFLOAT4 Color;

	Light(XMVECTOR Position, XMVECTOR Forward, XMVECTOR LightColor, float Intensity, float Radius);


};

class LightShadow
{
public:
	Light* Parent;
	XMFLOAT4X4 View;
	XMFLOAT4X4 Projection;

	LightShadow(XMVECTOR Position, XMVECTOR At, XMVECTOR Up, int Width, int Height, float FOV = XM_PI, float Near = 0.01f, float Far = 1000.0f);

	static void LightShadowDirectional(LightShadow* LS, XMVECTOR Position, XMVECTOR At, XMVECTOR Up, int Width, int Height, float Near = 0.01f, float Far = 1000.0f);
	static void LightShadowPoint(LightShadow* LS, XMVECTOR Position, XMVECTOR At, XMVECTOR Up, int Width, int Height, float FOV = XM_PI, float Near = 0.01f, float Far = 1000.0f);
};

