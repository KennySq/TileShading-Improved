#include "stdafx.h"
#include "Light.h"

Light::Light(XMVECTOR InPosition, XMVECTOR InForwardTo,XMVECTOR InLightColor, float InIntensity, float Radius)
{
	XMStoreFloat4(&Position, InPosition);
	Position.w = InIntensity;
	Color.w = Radius;
	XMVECTOR Direction = XMVector3Normalize(InPosition - InForwardTo);
	
	XMStoreFloat4(&Color, InLightColor);
	Color.w = Radius;
	XMStoreFloat4(&Forward, Direction);
}

LightShadow::LightShadow(XMVECTOR Position, XMVECTOR At, XMVECTOR Up, int Width, int Height, float FOV, float Near, float Far)
{
	XMStoreFloat4x4(&View, XMMatrixLookAtLH(Position, At, Up));
	XMStoreFloat4x4(&Projection, XMMatrixPerspectiveFovLH(FOV, (float)Width / (float)Height, Near, Far));
}

void LightShadow::LightShadowDirectional(LightShadow* LS, XMVECTOR Position, XMVECTOR At, XMVECTOR Up, int Width, int Height, float Near, float Far)
{
	XMStoreFloat4x4(&LS->Projection, XMMatrixOrthographicLH(Width, Height, Near, Far));
	XMStoreFloat4x4(&LS->View, XMMatrixLookAtLH(Position, At, Up));
}
