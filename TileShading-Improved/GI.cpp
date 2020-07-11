#include "stdafx.h"
#include "GI.h"

GI::GI(ID3D11Device* Device, UINT Width, UINT Height, Camera* ViewCamera)
{
	Rays.resize(Width * Height);
	MainCam = ViewCamera;
	XMFLOAT4 CamPos;
	XMFLOAT4 UVPos;

	XMStoreFloat4(&CamPos, ViewCamera->GetCameraPosition());

	XMVECTOR X = XMVectorSet(2.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Y = XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f);

	GIMap = new RenderTarget2D();
	RayPositionMap = new RenderTarget2D();
	RayDirectionMap = new RenderTarget2D();
	RayResultMap = new ComputeTexture2D();

	CreateScreenAlignedQuad(Device, &Quad);
	//Quad->ModelMaterial->OverridePass(Device, "Sample", "PathTrace.hlsl", "Path", (PASSTYPE)(PASSTYPE_VERTEX | PASSTYPE_GEOMETRY));
	ResourceManager::AllocModelBuffer<UVVertex>(Device, &Quad->ModelBuffer, Quad->ModelMesh);
	ResourceManager::AllocCustomBuffer<Ray>(Device, &RayBuffer, Rays.size(), (D3D11_BIND_FLAG)(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT), D3D11_USAGE_DEFAULT, 0,(void**)Rays.data());
	ResourceManager::AllocCustomBuffer<Ray>(Device, &RayBackBuffer, Rays.size(), (D3D11_BIND_FLAG)(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT), D3D11_USAGE_DEFAULT, 0, (void**)Rays.data());

	ResourceManager::AllocCustomBuffer<GISetting>(Device, &GISettingBuffer, 1, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DEFAULT,0);

	for (int i = 0; i < Height; i++)

	{
		for (int j = 0; j < Width; j++)
		{
			auto U = double(j) / (Width - 1);
			auto V = double(i) / (Height - 1);

			XMStoreFloat4(&UVPos, U * X + V * Y);

			Ray r = Ray(CamPos, UVPos);

			Rays[Width * i + j] = r;
		}
	}


	D3D11_TEXTURE2D_DESC MapDesc{};
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};

	MapDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	MapDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	MapDesc.Width = Width;
	MapDesc.Height = Height;
	MapDesc.SampleDesc.Count = 1;
	MapDesc.ArraySize = 1;

	RTVDesc.Format = MapDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	SRVDesc.Format = MapDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	HRESULT Result;
	Result = Device->CreateTexture2D(&MapDesc, nullptr, &GIMap->Tex);
	assert(Result == S_OK);

	Result = Device->CreateRenderTargetView(GIMap->Tex, &RTVDesc, &GIMap->RTV);
	assert(Result == S_OK);

	Result = Device->CreateShaderResourceView(GIMap->Tex, &SRVDesc, &GIMap->SRV);
	assert(Result == S_OK);

	D3D11_TEXTURE2D_DESC RayDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC RaySRVDesc{};
	D3D11_UNORDERED_ACCESS_VIEW_DESC RayUAVDesc{};

	RayDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	RayDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	RayDesc.Width = Width;
	RayDesc.Height = Height;
	RayDesc.SampleDesc.Count = 1;
	RayDesc.Usage = D3D11_USAGE_DEFAULT;
	RayDesc.ArraySize = 1;
	
	RaySRVDesc.Format = RayDesc.Format;
	RaySRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	RaySRVDesc.Texture2D.MipLevels = 1;

	RayUAVDesc.Format = RayDesc.Format;
	RayUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	
	Result = Device->CreateTexture2D(&RayDesc, nullptr, &RayResultMap->Tex);
	assert(Result == S_OK);

	Result = Device->CreateShaderResourceView(RayResultMap->Tex, &RaySRVDesc, &RayResultMap->SRV);
	assert(Result == S_OK);

	Result = Device->CreateUnorderedAccessView(RayResultMap->Tex, &RayUAVDesc, &RayResultMap->UAV);
	assert(Result == S_OK);

	/******************************************/

	SimpleTextureCS = new Material();

	SimpleTextureCS->CompilePass(Device, "CSTextureWrite.hlsl", "TextureWrite", PASSTYPE_COMPUTE);

	ResourceManager::AllocCustomBuffer<Light>(Device, &SceneLightsBuffer, MAX_PIXLIGHTS, (D3D11_BIND_FLAG)(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS), D3D11_USAGE_DEFAULT, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

	ResourceManager::CreateComputeTexture2D(Device, &DebugTiles, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, Width, Height, D3D11_USAGE_DEFAULT);
	DebugTilesCS = new Material();
	DebugTilesCS->CompilePass(Device, "ComputeTile.hlsl", "ComputeTile", PASSTYPE_COMPUTE);
	
	ResourceManager::CreateComputeTexture3D(Device, &ViewLight, DXGI_FORMAT_R32G32_UINT, 0, 40, 22, 16, D3D11_USAGE_DEFAULT);
}

GI::~GI()
{
	Rays.clear();
}
