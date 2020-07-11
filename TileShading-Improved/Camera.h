#pragma once

class Camera
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;

	float FOV = 0.0f;
	float AspectRatio = 0.0f;
	float Near = 0.0f;
	float Far = 0.0f;
public:

	XMMATRIX& GetView() { return View; }
	XMMATRIX& GetProjection() { return Projection; }

	void Moveable(float x, float y, float z);
	void Rotate(float x, float y, float z, bool ZLock = false);
	Camera(float FOV_, float AR_, float Near_, float Far_);
	static Camera* MakePerspectiveCamera(float FOV_, float AR_, float Near_, float Far_);

	_inline XMVECTOR GetCameraPosition()
	{
		XMVECTOR Det;
		XMMATRIX InvMat;
		InvMat = XMMatrixInverse(&Det, View);

		Det = XMVectorSet(InvMat.r[3].m128_f32[0], InvMat.r[3].m128_f32[1], InvMat.r[3].m128_f32[2], InvMat.r[3].m128_f32[3]);

		return Det;
	}

	_inline XMVECTOR GetCameraDirection()
	{
		XMFLOAT4X4 Temp;
		XMVECTOR Ret;
		XMStoreFloat4x4(&Temp, View);

		Ret.m128_f32[0] = Temp._13;
		Ret.m128_f32[1] = Temp._23;
		Ret.m128_f32[2] = Temp._33;
		return XMVector3Normalize(Ret);
	}
};