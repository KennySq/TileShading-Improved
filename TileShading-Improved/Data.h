#pragma once


#ifndef _DATA_H_
#define _DATA_H_

using namespace DirectX;

struct MeshBuffer
{
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
	ID3D11Buffer* ConstBuffer = nullptr;
	ID3D11Buffer* TriangleBuffer = nullptr;
};

struct Matrices
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
};

struct TileSettings
{
	bool VisualizeTiles = false;
	XMFLOAT3 Padding;
};
#endif