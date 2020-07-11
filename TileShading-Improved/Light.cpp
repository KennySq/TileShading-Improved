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
