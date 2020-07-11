#include "stdafx.h"
#include "ResourceManager.h"

HRESULT ResourceManager::CreateTexture2D(ID3D11Device* Device, DXGI_FORMAT Format, UINT Width, UINT Height, RenderTarget2D* pOutTexture)
{
	HRESULT Result;
	D3D11_TEXTURE2D_DESC TexDesc{};
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};

	TexDesc.Format = Format;
	TexDesc.Width = Width;
	TexDesc.Height = Height;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TexDesc.ArraySize = 1;

	RTVDesc.Format = Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	
	SRVDesc.Format = Format;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	Result = Device->CreateTexture2D(&TexDesc, nullptr, &pOutTexture->Tex);
	assert(Result == S_OK);

	Result = Device->CreateRenderTargetView(pOutTexture->Tex, &RTVDesc, &pOutTexture->RTV);
	assert(Result == S_OK);

	Result = Device->CreateShaderResourceView(pOutTexture->Tex, &SRVDesc, &pOutTexture->SRV);
	assert(Result == S_OK);

	return S_OK;
}

HRESULT ResourceManager::CreateRenderTarget2D(ID3D11Device* Device, RenderTarget2D** pRT, DXGI_FORMAT Format, UINT BindFlags, UINT Width, UINT Height, UINT Usage, UINT CPUAccessFlag, UINT ArraySize)
{
	HRESULT Result;
	D3D11_TEXTURE2D_DESC Desc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};

	if (*pRT == nullptr)
		*pRT = new RenderTarget2D();

	Desc.Format = Format;
	Desc.ArraySize = ArraySize;
	Desc.BindFlags = BindFlags | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	Desc.Usage = (D3D11_USAGE)Usage;
	Desc.Width = Width;
	Desc.Height = Height;
	Desc.SampleDesc.Count = 1;
	Desc.CPUAccessFlags = CPUAccessFlag;
	
	SRVDesc.Format = Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	RTVDesc.Format = Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	Result = Device->CreateTexture2D(&Desc, nullptr, &pRT[0]->Tex);
	assert(Result == S_OK && "Failed to Create ID3D11Texture2D");

	Result = Device->CreateRenderTargetView(pRT[0]->Tex, &RTVDesc, &pRT[0]->RTV);
	assert(Result == S_OK && "Failed to Create ID3D11RenderTargetView");

	Result = Device->CreateShaderResourceView(pRT[0]->Tex, &SRVDesc, &pRT[0]->SRV);
	assert(Result == S_OK && "Failed to Create ID3D11ShaderResourceView");

	return S_OK;
}

HRESULT ResourceManager::CreateComputeTexture2D(ID3D11Device* Device, ComputeTexture2D** pRT, DXGI_FORMAT Format, UINT BindFlags, UINT Width, UINT Height, UINT Usage, UINT CPUAccessFlag, UINT ArraySize)
{
	HRESULT Result;
	D3D11_TEXTURE2D_DESC Desc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};

	if (*pRT == nullptr)
		*pRT = new ComputeTexture2D();

	Desc.Format = Format;
	Desc.ArraySize = ArraySize;
	Desc.BindFlags = BindFlags | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Desc.Usage = (D3D11_USAGE)Usage;
	Desc.Width = Width;
	Desc.Height = Height;
	Desc.SampleDesc.Count = 1;
	Desc.CPUAccessFlags = CPUAccessFlag;

	SRVDesc.Format = Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	UAVDesc.Format = Format;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	
	Result = Device->CreateTexture2D(&Desc, nullptr, &pRT[0]->Tex);
	assert(Result == S_OK && "Failed to Create ID3D11Texture2D");

	Result = Device->CreateUnorderedAccessView(pRT[0]->Tex, &UAVDesc, &pRT[0]->UAV);
	assert(Result == S_OK && "Failed to Create ID3D11UnorderedAccessView");

	Result = Device->CreateShaderResourceView(pRT[0]->Tex, &SRVDesc, &pRT[0]->SRV);
	assert(Result == S_OK && "Failed to Create ID3D11ShaderResourceView");

	return S_OK;
}

HRESULT ResourceManager::CreateComputeTexture3D(ID3D11Device* Device, ComputeTexture3D** pCT, DXGI_FORMAT Format, UINT BindFlags, UINT Width, UINT Height, UINT Depth, UINT Usage, UINT CPUAccessFlag, UINT ArraySize)
{
	HRESULT Result;
	D3D11_TEXTURE3D_DESC TexDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};

	if (*pCT == nullptr)
		*pCT = new ComputeTexture3D();

	TexDesc.Format = Format;
	TexDesc.BindFlags = BindFlags | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	TexDesc.Depth = Depth;
	TexDesc.Width = Width;
	TexDesc.Height = Height;
	TexDesc.Usage = (D3D11_USAGE)Usage;
	TexDesc.CPUAccessFlags = CPUAccessFlag;
	
	
	SRVDesc.Format = Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	SRVDesc.Texture3D.MipLevels = 1;
	
	UAVDesc.Format = Format;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	UAVDesc.Texture3D.WSize = Depth;
	
	Result = Device->CreateTexture3D(&TexDesc, nullptr, &pCT[0]->Tex);
	assert(Result == S_OK && "Failed to create texture 3d");

	Result = Device->CreateShaderResourceView(pCT[0]->Tex, &SRVDesc, &pCT[0]->SRV);
	assert(Result == S_OK && "Failed to create shader resource view");

	Result = Device->CreateUnorderedAccessView(pCT[0]->Tex, &UAVDesc, &pCT[0]->UAV);
	assert(Result == S_OK && "Failed to create unordered access view");

	return S_OK;
}
