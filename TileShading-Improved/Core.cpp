#include "stdafx.h"
#include "Core.h"

Core::~Core()
{}

HRESULT Core::PostInitialize()
{
	UINT DeviceFlag = 0;

	HRESULT Result;

#ifdef _DEBUG
	DeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL FeatureLevel[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
	};

	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferCount =1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.Height = Height;
	SwapChainDesc.BufferDesc.Width = Width;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.OutputWindow = WindowHandle;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Windowed = true;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.Flags = DeviceFlag;

	Result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, DeviceFlag, nullptr, 0, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, &FeatureLevel[0], &Context);
	if (FAILED(Result))
		return Result;

	MainCam = Camera::MakePerspectiveCamera(XMConvertToRadians(90.0f) , 1.777f, 0.01f, 1000.0f);
	IM = new InputManager();
	IM->Intialize(HandleInstance, MainCam, Width, Height);


	GIManager = new GI(Device, Width, Height, MainCam);
	
	D3D11_RASTERIZER_DESC RasterDesc = CD3D11_RASTERIZER_DESC();
	RasterDesc.CullMode = D3D11_CULL_NONE;
	RasterDesc.FrontCounterClockwise = false;
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	Result = Device->CreateRasterizerState(&RasterDesc, &DefaultRasterizerState);
	assert(Result == S_OK && "Failed to create rasterizer state.");



	ImGui::SetCurrentContext(ImGui::CreateContext());

	IMGUI_CHECKVERSION();
	ImGuiIO IO	= ImGui::GetIO();
	
	ImGui_ImplDX11_Init(Device, Context);
	ImGui_ImplWin32_Init(WindowHandle);

	return S_OK;
}

HRESULT Core::Intialize()
{
	Resize();
	ResizeGBuffer();
	CreateScreenAlignedQuad(Device, &ScreenQuad);
	CreateSphere(Device, 3.0f, 32,32, &SampleModel);
	CreateCube(Device, &SampleModel2);
	CreatePlane(Device, 8, &SampleModel3);


	ResourceManager::AllocModelBuffer<UVVertex>(Device, &ScreenQuad->ModelBuffer, ScreenQuad->ModelMesh);
	ResourceManager::AllocModelBuffer<Vertex>(Device, &SampleModel->ModelBuffer, SampleModel->ModelMesh);
	ResourceManager::AllocModelBuffer<Vertex>(Device, &SampleModel2->ModelBuffer, SampleModel2->ModelMesh);
	ResourceManager::AllocModelBuffer<Vertex>(Device, &SampleModel3->ModelBuffer, SampleModel3->ModelMesh);

	ResourceManager::AllocCustomConstBuffer<Transform>(Device, &SampleModel->ModelBuffer);
	ResourceManager::AllocCustomConstBuffer<Transform>(Device, &SampleModel2->ModelBuffer);
	ResourceManager::AllocCustomConstBuffer<Transform>(Device, &SampleModel3->ModelBuffer);

	ResourceManager::AllocCustomConstBuffer<ViewProject>(Device, &ViewProjectionBuffer);

	ResourceManager::AllocCustomConstBuffer<TileSettings>(Device, &TileSettingBuffer);

	//SampleModel->ModelMaterial->OverridePass(Device, "DrawBuffer", "PathTrace.hlsl", "Path",
	//	(PASSTYPE)(PASSTYPE_VERTEX | PASSTYPE_GEOMETRY | PASSTYPE_FRAGMENT));
	//SampleModel2->ModelMaterial->OverridePass(Device, "DrawBuffer", "PathTrace.hlsl", "Path",
	//	(PASSTYPE)(PASSTYPE_VERTEX | PASSTYPE_GEOMETRY | PASSTYPE_FRAGMENT));
	//SampleModel3->ModelMaterial->OverridePass(Device, "DrawBuffer", "PathTrace.hlsl", "Path",
	//	(PASSTYPE)(PASSTYPE_VERTEX | PASSTYPE_GEOMETRY | PASSTYPE_FRAGMENT));


	string RayBufferName = "RayBuffer";
	string CSSampleName = "CS Test Sample";
	string GISettingName = "GI Setting";
	GIManager->RayBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, RayBufferName.size(), RayBufferName.c_str());
	GIManager->RayResultMap->Tex->SetPrivateData(WKPDID_D3DDebugObjectName, CSSampleName.size(), CSSampleName.c_str());
	GIManager->GISettingBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, GISettingName.size(), GISettingName.c_str());
	srand((unsigned)time(NULL));
	for (int i = 0; i < MAX_PIXLIGHTS; i++)
	{
		XMVECTOR Position = XMVectorSet((rand() %50) - 25, rand() % 30, (rand() % 50)-25, 1.0f);
		XMVECTOR Color = XMVector3Normalize(XMVectorSet(rand(), rand(), rand(), 1.0f));
 		AddLight(Light({ Position }, { 0.0f,0.0f,0.0f,1.0f }, Color, 1.0f, 10.0f));

	}

	//AddLight(Light({ 0.0f,5.0f,0.0,1.0f }, { 0.0f,0.0f,0.0f,1.0f }, {1.0f,0.0f,0.0f,0.0f}, 4.0f, 10.0f));
	//AddLight(Light({ 5.0f,7.0f,0.0,1.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 0.0f,1.0f,0.0f,0.0f }, 4.0f, 10.0f));
	//AddLight(Light({ 7.0f,5.0f,5.0,1.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 0.0f,0.0f,1.0f,0.0f }, 4.0f, 10.0f));
	//AddLight(Light({0.0f,5.0f,7.0,1.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,0.0f }, 0.5f, 10.0f));
	
	//AddLight(Light({ 25.0f,5.0f,0.0,1.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 1.0f,0.0f,0.0f,0.0f }, 4.0f, 10.0f));
	//AddLight(Light({ 25.0f,7.0f,0.0,1.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 0.0f,1.0f,0.0f,0.0f }, 4.0f, 10.0f));
	//AddLight(Light({ 27.0f,5.0f,25.0,1.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 0.0f,0.0f,1.0f,0.0f }, 4.0f, 10.0f));
	//AddLight(Light({ 0.0f,5.0f,27.0,1.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,0.0f }, 0.5f, 10.0f));

	
	
	//AddLight(Light({ 0.0f,50.0f,50.0,1.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 1.0f,1.0f,1.0f,0.0f }, 0.5f, 0.05f));

	WaveFrontReader<Index> SceneMesh;
	SponzaModel = new Model<Vertex>();

	HRESULT Result;
	//SponzaModel->ModelMesh->Indices.reserve()
	Result = SceneMesh.Load(L"C:\\Users\\pencu\\source\\repos\\TileShading-Improved\\Resources\\Models\\Sponza\\sponza.obj");
	
	assert(Result == S_OK);
	*SponzaModel = SceneMesh;

	SponzaModel->ModelMaterial->CompilePass(Device, "RenderScreen.hlsl", "DrawBuffer", (PASSTYPE)(PASSTYPE_VERTEX | PASSTYPE_FRAGMENT));
	ResourceManager::AllocModelBuffer<Vertex>(Device, &SponzaModel->ModelBuffer, SponzaModel->ModelMesh);
	ResourceManager::AllocCustomConstBuffer<Transform>(Device, &SponzaModel->ModelBuffer);

 	return S_OK;
}

void Core::Update(float Delta)
{
	static ID3D11RenderTargetView* const NullRTV[1] = { nullptr };
	static ID3D11UnorderedAccessView* NullUAV[1] = { nullptr };
	static ID3D11ShaderResourceView* NullSRV[1] = { nullptr };
	static ID3D11Buffer* NullBuffer[1] = { nullptr };

	IM->Update(Delta);
	ReleaseRenderTarget();
	ClearScreen(Colors::Black);
	ClearTexture(&TextureBuffers[0], Colors::Black);
	ClearTexture(&TextureBuffers[1], Colors::Black);
	ClearTexture(&TextureBuffers[2], Colors::Black);
	ClearTexture(&TextureBuffers[3], Colors::Black);
	Context->RSSetViewports(1, Viewport);
	Context->RSSetState(DefaultRasterizerState);

	DrawModel<Vertex>(SponzaModel, "DrawBuffer", XMVectorSet(0.0f,0.0f,0.0f,1.0f), XMVectorSet(0.1f,0.1f,0.1f,1.0f));
	//DrawModel<Vertex>(SampleModel, "DrawBuffer");
	//DrawModel<Vertex>(SampleModel2, "DrawBuffer", XMVectorSet(10.0f, 0.0f, 0.0f, 0.0f));
	//DrawModel<Vertex>(SampleModel3, "DrawBuffer", XMVectorSet(0.0f, -2.0f, 0.0f, 0.0f));
	ComputeTiles();

	

	DrawScreen();
	Context->ClearState();
	SetRenderToDirect();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	ImGui::Begin("Hello");

	ImGui::Checkbox("Visualize Tile", &TileOptions.VisualizeTiles);

	XMVECTOR Pos = MainCam->GetCameraPosition();

	ImGui::Text("(%f, %f, %f)", Pos.m128_f32[0], Pos.m128_f32[1], Pos.m128_f32[2]);
	ImGui::End();
	ImGui::EndFrame();


}

void Core::Render(float Delta)
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ReleaseRenderTarget();
	SwapChain->Present(0, 0);





}

void Core::Release()
{
	ReleaseBuffer();

	Device->Release();
	Context->Release();
	SwapChain->Release();
	
}

template<class _VertTy>
void Core::IABindBuffer(Model<_VertTy>* BindModel,string PassName, D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	static const UINT Stride = sizeof(_VertTy);
	static const UINT Offset = 0;
	Context->IASetInputLayout(BindModel->ModelMaterial->GetPass(PassName)->IL);
	Context->IASetVertexBuffers(0, 1, &BindModel->ModelBuffer->VertexBuffer, &Stride, &Offset);
	Context->IASetPrimitiveTopology(Topology);

#ifdef _INDEX16
	Context->IASetIndexBuffer(BindModel->ModelBuffer->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
#endif
#ifdef _INDEX32
	Context->IASetIndexBuffer(BindModel->ModelBuffer->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
#endif

}

template<class _VertTy>
void Core::DrawModel(Model<_VertTy>* InputModel, string PassName, XMVECTOR Offset, XMVECTOR Size)
{
	static ID3D11RenderTargetView* GBuffers[] = { TextureBuffers[0].RTV, TextureBuffers[1].RTV, TextureBuffers[2].RTV, TextureBuffers[3].RTV };
	static ID3D11DepthStencilView* Depth = DepthStencil->DSV;
	static ID3D11RenderTargetView* NullRTV[1] = { nullptr };
	static ID3D11VertexShader* VS = InputModel->ModelMaterial->GetPass(PassName.c_str())->VS;
	static ID3D11PixelShader* PS = InputModel->ModelMaterial->GetPass(PassName.c_str())->PS;
	//static ID3D11GeometryShader* GS = InputModel->ModelMaterial->GetPass(PassName.c_str())->GS;
	
	static Transform Mat;


	XMStoreFloat4x4(&Mat.World, XMMatrixIdentity());
	Mat.World._14 = Offset.m128_f32[0];
	Mat.World._24 = Offset.m128_f32[1];
	Mat.World._34 = Offset.m128_f32[2];

	Mat.World._11 = Size.m128_f32[0];
	Mat.World._22 = Size.m128_f32[1];
	Mat.World._33 = Size.m128_f32[2];
	Mat.World._44 = Size.m128_f32[3];

	XMStoreFloat4x4(&Mat.View, XMMatrixTranspose(MainCam->GetView()));
	XMStoreFloat4x4(&Mat.Projection, XMMatrixTranspose(MainCam->GetProjection()));

	Context->UpdateSubresource(InputModel->ModelBuffer->ConstBuffer, 0, nullptr, &Mat, 0, 0);

	Context->OMSetRenderTargets(4, GBuffers, Depth);
	
	Context->VSSetShader(VS, nullptr, 0);
//	Context->GSSetShader(GS, nullptr, 0);
	Context->PSSetShader(PS, nullptr, 0);
	
	IABindBuffer<Vertex>(InputModel, PassName.c_str(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Context->VSSetConstantBuffers(0, 1, &InputModel->ModelBuffer->ConstBuffer);
	Context->DrawIndexed(InputModel->ModelMesh->Indices.size(), 0, 0);
	
	Context->OMSetRenderTargets(1, NullRTV, nullptr);


}

void Core::ComputeTiles()
{
	static ID3D11UnorderedAccessView* const NullUAV[1] = { nullptr };
	static ID3D11ShaderResourceView*const NullSRV[1] = { nullptr};
	static ID3D11Buffer* NullBuffer[1] = { nullptr };
	static ID3D11ComputeShader* NullCS[1] = { nullptr };
	static ID3D11ShaderResourceView* ResourcesSRV[] = { 
		TextureBuffers[0].SRV,
		TextureBuffers[1].SRV, 
		TextureBuffers[3].SRV,
		GIManager->SceneLightSRV,
	};
	static ID3D11UnorderedAccessView* InputUAV[] = { GIManager->DebugTiles->UAV , GIManager->ViewLight->UAV };
	static ID3D11Buffer* BindConstBuffers[] = { ViewProjectionBuffer, TileSettingBuffer };
	
	//static ID3D11Buffer* BindBuffer[] = { GIManager->SceneLightSRV };
	static unsigned int ArrSize = ARRAYSIZE(ResourcesSRV);
	unsigned int UAVSize = ARRAYSIZE(InputUAV);

	ViewProject VP;

	XMStoreFloat4x4(&VP.View, XMMatrixTranspose(MainCam->GetView()));
	XMStoreFloat4x4(&VP.Projection, XMMatrixTranspose(MainCam->GetProjection()));

	Context->UpdateSubresource(ViewProjectionBuffer, 0, nullptr, &VP, 0, 0);

	Context->UpdateSubresource(TileSettingBuffer, 0, nullptr, &TileOptions, 0, 0);
	

	Context->CSSetShader(GIManager->DebugTilesCS->GetPass("ComputeTile")->CS, nullptr, 0);
	Context->CSSetUnorderedAccessViews(0, UAVSize, InputUAV, nullptr);
	Context->CSSetShaderResources(0, ArrSize, ResourcesSRV);
	Context->CSSetConstantBuffers(0, 2, BindConstBuffers);

	Context->Dispatch(80, 45, 1);
	Context->CSSetShaderResources(0, 1, NullSRV);
	Context->CSSetConstantBuffers(0, 1, NullBuffer);
	Context->CSSetUnorderedAccessViews(0, UAVSize, NullUAV, nullptr);

}

void Core::DrawScreen()
{
	static ID3D11ShaderResourceView* GBuffers[] = { 
		TextureBuffers[0].SRV, // WorldPosition
		TextureBuffers[1].SRV, // WorldNormal
		TextureBuffers[2].SRV, // UV
		TextureBuffers[3].SRV, // ViewPosition
		GIManager->SceneLightSRV,
		GIManager->ViewLight->SRV,
		GIManager->DebugTiles->SRV
	};

	static ID3D11VertexShader* VS = ScreenQuad->ModelMaterial->GetPass("Sample")->VS;
	static ID3D11PixelShader* PS = ScreenQuad->ModelMaterial->GetPass("Sample")->PS;
	static ID3D11ShaderResourceView* NullSRV[1] = { nullptr };
	static ID3D11RenderTargetView* NullRTV[1] = {nullptr};

	Context->CSSetShaderResources(0, 1, NullSRV);
	Context->PSSetShaderResources(0, 1, NullSRV);

	Context->VSSetShader(VS, nullptr, 0);
	Context->PSSetShader(PS, nullptr, 0);
	Context->PSSetShaderResources(0, ARRAYSIZE(GBuffers), GBuffers);
	
	IABindBuffer<UVVertex>(ScreenQuad, "Sample", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context->OMSetRenderTargets(1, &SwapChainBuffer->RTV, nullptr);

	Context->DrawIndexed(6, 0, 0);
	Context->CSSetShaderResources(0, 1, NullSRV);
	Context->PSSetShaderResources(0,1,NullSRV);
	Context->OMSetRenderTargets(1, NullRTV, nullptr);
}

void Core::AddLight(Light NewLight)
{


	HRESULT Result;
	D3D11_SHADER_RESOURCE_VIEW_DESC SceneLightSRVDesc{};

	XMVECTOR ViewPosition;

	//NewLight.Position.w = 1.0f;
	ViewPosition = XMLoadFloat4(&NewLight.Position);
	
	XMStoreFloat4(&NewLight.Forward, XMVector3TransformCoord(ViewPosition, MainCam->GetView()));

	SceneLights.push_back(NewLight);



	if (SceneLightSRVDesc.Buffer.NumElements > MAX_PIXLIGHTS)
		assert(true && "Reached Maximum Pixel Lights!");

	if (GIManager->SceneLightSRV != nullptr)
	{
		GIManager->SceneLightSRV->GetDesc(&SceneLightSRVDesc);
		SceneLightSRVDesc.Buffer.NumElements++;

		GIManager->SceneLightSRV->Release();
		GIManager->SceneLightSRV = nullptr;
	}
	else
	{
		SceneLightSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		SceneLightSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		SceneLightSRVDesc.Buffer.NumElements = 1;
	}

	Context->UpdateSubresource(GIManager->SceneLightsBuffer, 0, nullptr, SceneLights.data(), 0, 0);


	Result = Device->CreateShaderResourceView(GIManager->SceneLightsBuffer, &SceneLightSRVDesc, &GIManager->SceneLightSRV);
	assert(Result == S_OK);

	return;
}

HRESULT Core::Resize()
{
	HRESULT Result;
	static D3D11_TEXTURE2D_DESC TexDesc{};
	static D3D11_TEXTURE2D_DESC DepthDesc{};
	static D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
	static D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	static D3D11_SHADER_RESOURCE_VIEW_DESC DepthSRVDesc{};
	static D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc{};
	
	ReleaseBuffer();

	SwapChainBuffer = new RenderTarget2D();
	DepthStencil = new DepthStencil2D();
	Viewport = new D3D11_VIEWPORT();


	TexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TexDesc.Height = Height;
	TexDesc.Width = Width;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.SampleDesc.Count = 1;

	DepthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	DepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	DepthDesc.Height = Height;
	DepthDesc.Width = Width;
	DepthDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthDesc.SampleDesc.Count = 1;
	DepthDesc.ArraySize = 1;

	DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Texture2D.MipSlice = 0;


	DepthSRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	DepthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	DepthSRVDesc.Texture2D.MipLevels = 1;
	DepthSRVDesc.Texture2D.MostDetailedMip = 0;

	Viewport->Width = (float)Width;
	Viewport->Height = (float)Height;
	Viewport->MinDepth = 0.0f;
	Viewport->MaxDepth = 1.0f;
	Viewport->TopLeftX = 0.0f;
	Viewport->TopLeftY = 0.0f;

	Context->RSSetViewports(1, Viewport);

	RTVDesc.Format = TexDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	SRVDesc.Format = TexDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	Result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&SwapChainBuffer->Tex);
	if (FAILED(Result))
		return Result;

	Result = Device->CreateRenderTargetView(SwapChainBuffer->Tex, nullptr, &SwapChainBuffer->RTV);
	if (FAILED(Result))
		return Result;

	Result = Device->CreateTexture2D(&DepthDesc, nullptr, &DepthStencil->Tex);
	assert(Result == S_OK && "Failed to create depth texture.");

	Result = Device->CreateDepthStencilView(DepthStencil->Tex, &DSVDesc, &DepthStencil->DSV);
	assert(Result == S_OK && "Failed to create DSV");

	Result = Device->CreateShaderResourceView(DepthStencil->Tex, &DepthSRVDesc, &DepthStencil->SRV);
	assert(Result == S_OK && "Failed to create DS SRV");

	return S_OK;
}

HRESULT Core::ResizeGBuffer()
{
	ReleaseGBuffer();

	TextureBuffers = new RenderTarget2D[BufferCount];

	HRESULT Result;
	D3D11_TEXTURE2D_DESC TexDesc[] =
	{
		{Width, Height, 0, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, {1,0}, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,0, 0 },
		{Width, Height, 0, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, {1,0}, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,0, 0 },
		{Width, Height, 0, 1, DXGI_FORMAT_R32G32_FLOAT, {1,0}, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,0, 0 },
		{Width, Height, 0, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, {1,0}, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,0, 0 },

	};
	
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc[] =
	{
		{TexDesc[0].Format, D3D11_RTV_DIMENSION_TEXTURE2D},
		{TexDesc[1].Format, D3D11_RTV_DIMENSION_TEXTURE2D},
		{TexDesc[2].Format, D3D11_RTV_DIMENSION_TEXTURE2D},
		{TexDesc[3].Format, D3D11_RTV_DIMENSION_TEXTURE2D},
	};

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc[] =
	{
		{TexDesc[0].Format, D3D11_SRV_DIMENSION_TEXTURE2D, 0, 1},
		{TexDesc[1].Format, D3D11_SRV_DIMENSION_TEXTURE2D, 0, 1},
		{TexDesc[2].Format, D3D11_SRV_DIMENSION_TEXTURE2D, 0, 1},
		{TexDesc[3].Format, D3D11_SRV_DIMENSION_TEXTURE2D, 0, 1},
	};

	for (UINT i = 0; i < BufferCount; i++)
	{
		Result = Device->CreateTexture2D(&TexDesc[i], nullptr, &TextureBuffers[i].Tex);
		assert(Result == S_OK || Result == S_FALSE && "Failed to create gbuffer texture.");

		Result = Device->CreateRenderTargetView(TextureBuffers[i].Tex, &RTVDesc[i], &TextureBuffers[i].RTV);
		assert(Result == S_OK || Result == S_FALSE && "Failed to create gbuffer RTV.");

		Result = Device->CreateShaderResourceView(TextureBuffers[i].Tex, &SRVDesc[i], &TextureBuffers[i].SRV);
		assert(Result == S_OK || Result == S_FALSE && "Failed to create gbuffer SRV.");

	}

	return S_OK;
}

void Core::ReleaseBuffer()
{
	if (SwapChainBuffer)
	{
		SwapChainBuffer->Tex->Release();
		SwapChainBuffer->RTV->Release();

		SwapChainBuffer->Tex = nullptr;
		SwapChainBuffer->RTV = nullptr;
	}

	delete SwapChainBuffer;
	SwapChainBuffer = nullptr;

	delete Viewport;
	Viewport = nullptr;

	ReleaseGBuffer();
	delete[] TextureBuffers;

	

#ifndef _DEFERRED

	if (DepthStencil)
	{
		DepthStencil->Tex->Release();
		DepthStencil->DSV->Release();
		DepthStencil->SRV->Release();

		DepthStencil->Tex = nullptr;
		DepthStencil->DSV = nullptr;
		DepthStencil->SRV = nullptr;
	}

	delete DepthStencil;

#else
	if (!GBuffer.empty())
	{
		for (int i = 0; i < BufferCount; i++)
		{
			GBuffer[i]->Tex->Release();

			GBuffer[i]->SRV->Release();

			if (i > 0)
			{
				GBuffer[i]->ToRenderTarget()->RTV->Release();

				GBuffer[i]->Tex = nullptr;
				GBuffer[i]->ToRenderTarget()->RTV = nullptr;
				GBuffer[i]->SRV = nullptr;
			}
			else
			{
				GBuffer[i]->ToDepthStencil()->DSV->Release();

				GBuffer[i]->Tex = nullptr;
				GBuffer[i]->ToDepthStencil()->DSV = nullptr;
				GBuffer[i]->SRV = nullptr;
			}


		}
	}

	GBuffer.clear();
	delete GBufferRTVPtr;


#endif



}

void Core::ReleaseGBuffer()
{
	if (TextureBuffers == nullptr)
		return;

	for (int i = 0; i < BufferCount; i++)
	{
		if(TextureBuffers[i].Tex == nullptr)
			TextureBuffers[i].Tex->Release();

		if (TextureBuffers[i].RTV == nullptr)
			TextureBuffers[i].RTV->Release();

		if (TextureBuffers[i].SRV == nullptr)
			TextureBuffers[i].SRV->Release();
	}
}
