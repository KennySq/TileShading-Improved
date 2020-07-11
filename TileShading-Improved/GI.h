#pragma once

struct LightProperties
{
	XMFLOAT4 Position;
	XMFLOAT4 Forward;

	float Intensity;
};

struct ComputeSampleBuffer
{
	unsigned int DispatchCount;
	XMFLOAT3 Padding;
};

struct GISetting
{
	XMFLOAT4 CameraPosition;
	XMFLOAT4 CameraDirection;
	XMFLOAT4 BWHP;
	XMFLOAT4 Padding;
	//UINT Bounce;

	//UINT Width;
	//UINT Height;

	//UINT Padding;
};

struct Ray
{
	XMFLOAT4 Direction = { 0.0f,0.0f,0.0f, 0.0f };
	float Scalar = 0.0f;

	Ray(XMFLOAT4 From, XMFLOAT4 To)
	{
		XMVECTOR P1V, P2V;

		P1V = XMLoadFloat4(&From);
		P2V = XMLoadFloat4(&To);
		
		XMStoreFloat4(&Direction, XMVector3Normalize(P2V - P1V));
	}
	Ray() {	}
};

class GI
{
	vector<Ray> Rays;
	Camera* MainCam;
	UINT PerPixel;

	GISetting Settings;

	RenderTarget2D* GIMap;

	RenderTarget2D* RayPositionMap;
	RenderTarget2D* RayDirectionMap;

	Model<UVVertex>* Quad;

	ID3D11Buffer* RayBackBuffer = nullptr;

public:
	ID3D11Buffer* RayBuffer = nullptr;
	ID3D11Buffer* GISettingBuffer = nullptr;

	ID3D11Buffer* SceneLightsBuffer = nullptr;
	ID3D11ShaderResourceView* SceneLightSRV = nullptr;

	ComputeTexture2D* RayResultMap;
	Material* SimpleTextureCS = nullptr;


	ComputeTexture2D* DebugTiles = nullptr;
	ComputeTexture3D* ViewLight = nullptr;

	Material* DebugTilesCS = nullptr;
	UINT GetBounceStep() { return Settings.BWHP.x; }
	UINT SetBounceStep(UINT NewStep) { Settings.BWHP.x = NewStep; }
	GI(ID3D11Device* Device, UINT Width, UINT Height, Camera* ViewCamera);
	~GI();

	
};

