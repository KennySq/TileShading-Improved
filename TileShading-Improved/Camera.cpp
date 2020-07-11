#include "stdafx.h"
#include "Camera.h"

void Camera::Moveable(float x,float y,float z)
{
	XMVECTOR Det;
	XMVECTOR Offset;
	XMMATRIX InvViewTmp;
	XMFLOAT4X4 InvView;
	InvViewTmp = XMMatrixInverse(&Det, View);

	XMStoreFloat4x4(&InvView, InvViewTmp);
	
	Offset = XMVectorSet(x, y, z, 0.0f);
	
	Offset = XMVector3TransformCoord(Offset, View);

	View *= XMMatrixTranslation(z, -y, -x);

}

void Camera::Rotate(float x, float y, float z, bool ZLock)
{
	XMVECTOR Det;
	XMVECTOR Offset;
	XMMATRIX InvViewTmp;
	XMFLOAT4X4 InvView;
	InvViewTmp = XMMatrixInverse(&Det, View);

	XMStoreFloat4x4(&InvView, InvViewTmp);

	if (ZLock)
	{
		//View *= XMMatrixRotationRollPitchYaw(-y, -x, 0.0f);
		//XMVECTOR Up = XMVectorSet(0.0f,1.0f,0.0f,0.0f);
		//XMVECTOR Right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		XMMATRIX Rot;
		//XMVECTOR Position = GetCameraPosition();
		//XMVECTOR Direction = GetCameraDirection();
		
		//Rot = XMMatrixLookToLH(Position, Direction, XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
		
		//XMMATRIX Yaw = XMMatrixRotationAxis(Right, -y);
		//XMMATRIX Pitch = XMMatrixRotationAxis(Up, -x);
		//XMMATRIX Roll = XMMatrixRotationAxis(;
		Rot = XMMatrixRotationRollPitchYaw(-y, -x, 0.0f);//XMMatrixRotationAxis(XMVectorSet(-y, -x, 0.0f, 1.0f), 2.0f);
		XMMATRIX Yaw = XMMatrixRotationX(-y);
		XMMATRIX Pitch = XMMatrixRotationY(-x);
		XMMATRIX Roll = XMMatrixRotationZ(0.0f);

		View *= Rot;
	}

	else
		View*=XMMatrixRotationRollPitchYaw(y, x, z);
}

Camera::Camera(float FOV_, float AR_, float Near_, float Far_)
{
	FOV = FOV_;
	AspectRatio = AR_;
	Near = Near_;
	Far = Far_;

	View = XMMatrixLookAtLH(XMVectorSet(5.0, 5.0f, 0.0f, 1.0f), 
					 XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
					 XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));

	Projection = XMMatrixPerspectiveFovLH(FOV_, AR_, Near_, Far_);

}

Camera* Camera::MakePerspectiveCamera(float FOV_, float AR_, float Near_, float Far_)
{
	return new Camera(FOV_, AR_, Near_, Far_);
}
