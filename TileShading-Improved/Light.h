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

