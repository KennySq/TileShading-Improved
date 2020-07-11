#pragma once

#include"stdafx.h"

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

struct RenderTarget2D;
struct DepthStencil2D;

struct Texture2D
{
	ID3D11Texture2D* Tex = nullptr;
	ID3D11ShaderResourceView* SRV = nullptr;

	DepthStencil2D* ToDepthStencil() { return (DepthStencil2D*)this; }
	RenderTarget2D* ToRenderTarget() { return (RenderTarget2D*)this; }
};

struct Texture3D
{
	ID3D11Texture3D* Tex = nullptr;
	ID3D11ShaderResourceView* SRV = nullptr;

	//RenderTarget3D* ToRenderTarget() { return (RenderTarget3D*)this; }
};

struct RenderTarget3D : public Texture3D
{
	ID3D11RenderTargetView* RTV = nullptr;
};

struct DepthStencil3D : public Texture3D
{
	ID3D11DepthStencilView* DSV = nullptr;
};

struct RenderTarget2D : public Texture2D
{
	ID3D11RenderTargetView* RTV = nullptr;
};

struct ComputeTexture2D : public Texture2D
{
	ID3D11UnorderedAccessView* UAV = nullptr;
};

struct ComputeTexture3D : public Texture3D
{
	ID3D11UnorderedAccessView* UAV = nullptr;
};

struct DepthStencil2D : public Texture2D
{
	ID3D11DepthStencilView* DSV = nullptr;
};






#endif