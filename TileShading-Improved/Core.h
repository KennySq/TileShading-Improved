#pragma once

#ifndef _CORE_H_
#define _CORE_H_

using namespace std;
using namespace DirectX;

class Core
{
	UINT Width;
	UINT Height;

	static const UINT BufferCount = 4;

	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* Context = nullptr;
	IDXGISwapChain* SwapChain = nullptr;

	HWND WindowHandle;
	HINSTANCE HandleInstance;

	RenderTarget2D* SwapChainBuffer = nullptr;
	D3D11_VIEWPORT* Viewport = nullptr;
	ID3D11RasterizerState* DefaultRasterizerState = nullptr;

	Model<UVVertex>* ScreenQuad = nullptr;
	Model<Vertex>* SampleModel = nullptr;
	Model<Vertex>* SampleModel2 = nullptr;
	Model<Vertex>* SampleModel3 = nullptr;
	Model<Vertex>* SponzaModel = nullptr;

	vector<Light> SceneLights;

	ID3D11Buffer* ViewProjectionBuffer = nullptr;

	RenderTarget2D* TextureBuffers = nullptr;
#ifndef _DEFERRED
	DepthStencil2D* DepthStencil = nullptr;
#endif

	TileSettings TileOptions;
	Camera* MainCam;
	InputManager* IM;
	GI* GIManager;
	
	ID3D11Buffer* TileSettingBuffer;
#ifdef _DEFERRED
	ID3D11RenderTargetView** GBufferRTVPtr = nullptr;
	vector<Texture2D*> GBuffer;
	UINT BufferCount = 0;
	vector<DXGI_FORMAT> GBufferFormats;
#endif


private:
	HRESULT Resize();
	HRESULT ResizeGBuffer();
	void ReleaseBuffer();

	void ReleaseGBuffer();

	

public:
	Core(HWND InWindowHandle, HINSTANCE InHandleInstance, UINT InWidth, UINT InHeight) : 
		WindowHandle(InWindowHandle), HandleInstance(InHandleInstance), Width(InWidth), Height(InHeight) { }
	~Core();

	HRESULT PostInitialize();

	HRESULT Intialize();
	void Update(float Delta);
	void Render(float Delta);
	void Release();

	void DrawGI();
	void DrawTextureCS();

	_inline void ClearScreen(XMVECTORF32 Color)
	{
		Context->ClearRenderTargetView(SwapChainBuffer->RTV, Color);
	#ifndef _DEFERRED
		Context->ClearDepthStencilView(DepthStencil->DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	#else
		for (int i = 1; i < BufferCount; i++)
		{
			Context->ClearRenderTargetView(GBufferRTVPtr[i], Color);
		}
		Context->ClearDepthStencilView(GBuffer[0]->ToDepthStencil()->DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	#endif

	}

	_inline void ClearTexture(RenderTarget2D* Texture,XMVECTORF32 Color, DepthStencil2D* Depth = nullptr)
	{
		Context->ClearRenderTargetView(Texture->RTV, Color);

		if(Depth != nullptr)
			Context->ClearDepthStencilView(Depth->DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	}

	void ComputeTiles();

	template<class _VertTy>
	void IABindBuffer(Model<_VertTy>* Buffer, string PassName, D3D11_PRIMITIVE_TOPOLOGY Topology);
	void DrawScreen();

	template<class _VertTy>
	void DrawModel(Model<_VertTy>* InputModel, string PassName ,XMVECTOR Offset = XMVectorSet(0.0f,0.0f,0.0f,1.0f), XMVECTOR Size = XMVectorSet(1.0f,1.0f,1.0f,1.0f));
	
	void AddLight(Light NewLight);

	_inline void SetRenderTarget(UINT Count, ID3D11RenderTargetView** RTV, ID3D11DepthStencilView* DSV = nullptr) { 
		

		if(DSV == nullptr)
			Context->OMSetRenderTargets(Count, RTV, nullptr);
		else
			Context->OMSetRenderTargets(Count, RTV, DSV);

	}

	_inline void ReleaseRenderTarget() { 
		static ID3D11RenderTargetView* RTV = nullptr; 
		Context->OMSetRenderTargets(1, &RTV, nullptr);
	}

#ifdef _FORWARD
	_inline void SetRenderToDirect() { Context->OMSetRenderTargets(1, &SwapChainBuffer->RTV, nullptr); }
#endif
#ifdef _DEFERRED
	_inline void SetRenderToDirect() { Context->OMSetRenderTargets(1, &SwapChainBuffer->RTV, nullptr); }
	_inline void SetRenderToGBuffer() { Context->OMSetRenderTargets(BufferCount, GBufferRTVPtr, GBuffer[0]->ToDepthStencil()->DSV); }

#endif

};

#endif