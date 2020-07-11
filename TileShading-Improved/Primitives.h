#pragma once

static HRESULT CreateScreenAlignedQuad(ID3D11Device* Device, Model<UVVertex>** pOutModel)
{
	*pOutModel = new Model<UVVertex>();
	pOutModel[0]->ModelMesh = new Mesh<UVVertex>();
	pOutModel[0]->ModelMaterial = new Material();

	UVVertex V[] =
	{
		{ {-1.0f, 1.0f, 0.0f,1.0f }, {0.0f, 0.0f}},
		{ {1.0f, 1.0f, 0.0f,1.0f }, {1.0f, 0.0f}},
		{ {-1.0f, -1.0f, 0.0f,1.0f }, {0.0f, 1.0f}},
		{ {1.0f, -1.0f, 0.0f,1.0f }, {1.0f, 1.0f}}
	};

	Index I[] =
	{
		0,1,2,
		1,3,2
	};

	pOutModel[0]->ModelMesh->Vertices.push_back(V[0]);
	pOutModel[0]->ModelMesh->Vertices.push_back(V[1]);
	pOutModel[0]->ModelMesh->Vertices.push_back(V[2]);
	pOutModel[0]->ModelMesh->Vertices.push_back(V[3]);

	pOutModel[0]->ModelMesh->Indices.push_back(I[0]);
	pOutModel[0]->ModelMesh->Indices.push_back(I[1]);
	pOutModel[0]->ModelMesh->Indices.push_back(I[2]);
	pOutModel[0]->ModelMesh->Indices.push_back(I[3]);
	pOutModel[0]->ModelMesh->Indices.push_back(I[4]);
	pOutModel[0]->ModelMesh->Indices.push_back(I[5]);

	pOutModel[0]->ModelMaterial->CompilePass(Device, "ScreenTexture.hlsl", "Sample", (PASSTYPE)(PASSTYPE_VERTEX | PASSTYPE_FRAGMENT));


	return S_OK;
}


static void CreateCube(ID3D11Device* Device, Model<Vertex>** pOutModel)
{
	*pOutModel = new Model<Vertex>();
	pOutModel[0]->ModelMesh = new Mesh<Vertex>();
	pOutModel[0]->ModelMaterial = new Material();

	Vertex V[]{
		{ XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f) , XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

		{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		{ XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		{ XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		{ XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};

	Index I[] =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	for (int i = 0; i < ARRAYSIZE(V); i++)
	{
		pOutModel[0]->ModelMesh->Vertices.push_back(V[i]);
	}

	for (int i = 0; i < ARRAYSIZE(I); i++)
	{
		pOutModel[0]->ModelMesh->Indices.push_back(I[i]);
	}

	pOutModel[0]->ModelMaterial->CompilePass(Device, "RenderScreen.hlsl", "DrawBuffer", (PASSTYPE)(PASSTYPE_VERTEX | PASSTYPE_FRAGMENT));

}

static void CreateSphere(ID3D11Device* Device, float Radius, UINT Slice, UINT Stack, Model<Vertex>** pOutModel)
{

	*pOutModel = new Model<Vertex>();

	pOutModel[0]->ModelMesh = new Mesh<Vertex>();
	pOutModel[0]->ModelMaterial = new Material();

	Vertex topVertex = {{ 0.0f, +Radius, 0.0f,1.0f }, { 0.0f, +1.0f, 0.0f }, { 0.0f, 0.0f } };
	Vertex bottomVertex = { { 0.0f, -Radius, 0.0f, 1.0f	}, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f} };

	pOutModel[0]->ModelMesh->Vertices.push_back(topVertex);

	float phiStep = XM_PI / Stack;
	float thetaStep = 2.0f * XM_PI / Slice;

	for (UINT i = 1; i <= Stack - 1; ++i)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= Slice; ++j)
		{
			float theta = j * thetaStep;

			Vertex v;

			v.Position.x = Radius * sinf(phi) * cosf(theta);
			v.Position.y = Radius * cosf(phi);
			v.Position.z = Radius * sinf(phi) * sinf(theta);

			// Partial derivative of P with respect to theta
			//v.TangentU.x = -Radius * sinf(phi) * sinf(theta);
			//v.TangentU.y = 0.0f;
			//v.TangentU.z = +Radius * sinf(phi) * cosf(theta);

			//XMVECTOR T = XMLoadFloat3(&v.TangentU);
			//XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat4(&v.Position);
			XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

			v.UV.x = theta / XM_2PI;
			v.UV.y = phi / XM_PI;

			pOutModel[0]->ModelMesh->Vertices.push_back(v);
		}
	}

	pOutModel[0]->ModelMesh->Vertices.push_back(bottomVertex);

	for (UINT i = 1; i <= Slice; ++i)
	{
		pOutModel[0]->ModelMesh->Indices.push_back(0);
		pOutModel[0]->ModelMesh->Indices.push_back(i + 1);
		pOutModel[0]->ModelMesh->Indices.push_back(i);
	}

	UINT baseIndex = 1;
	UINT ringVertexCount = Slice + 1;
	for (UINT i = 0; i < Stack - 2; ++i)
	{
		for (UINT j = 0; j < Slice; ++j)
		{
			pOutModel[0]->ModelMesh->Indices.push_back(baseIndex + i * ringVertexCount + j);
			pOutModel[0]->ModelMesh->Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			pOutModel[0]->ModelMesh->Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			pOutModel[0]->ModelMesh->Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			pOutModel[0]->ModelMesh->Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			pOutModel[0]->ModelMesh->Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	UINT southPoleIndex = (UINT)pOutModel[0]->ModelMesh->Vertices.size() - 1;

	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < Slice; ++i)
	{
		pOutModel[0]->ModelMesh->Indices.push_back(southPoleIndex);
		pOutModel[0]->ModelMesh->Indices.push_back(baseIndex + i);
		pOutModel[0]->ModelMesh->Indices.push_back(baseIndex + i + 1);
	}

	pOutModel[0]->ModelMaterial->CompilePass(Device, "RenderScreen.hlsl", "DrawBuffer", (PASSTYPE)(PASSTYPE_VERTEX | PASSTYPE_FRAGMENT));


}
static void CreatePlane(ID3D11Device* Device, int Length, Model<Vertex>** pOutModel)
{
	vector<Vertex> V;
	vector<Index> I;

	*pOutModel = new Model<Vertex>();

	pOutModel[0]->ModelMesh = new Mesh<Vertex>();
	pOutModel[0]->ModelMaterial = new Material();

	int p = 0;

	for (int i = 0; i < Length; i++) // z
	{
		for (int j = 0; j < Length; j++) // x
		{

			short i1, i2, i3, i4, i5, i6;

			i1 = p;

			i2 = i1 + 1;

			i3 = Length + i2;

			i4 = i3;
			i5 = i2;
			i6 = i4 + 1;

			I.push_back(i6);
			I.push_back(i5);
			I.push_back(i4);
			I.push_back(i3);
			I.push_back(i2);
			I.push_back(i1);

			p++;
		}
		p++;
	}

	for (int i = 0; i < p + (Length + 1); i++)
	{
		float Xoffset = i % (Length + 1);
		float Zoffset = i / (Length + 1);
		//float Yoffset = rand() % (int)Frequency;

		float u = Xoffset / Length;
		float v = Zoffset / Length;

		Vertex v1 = { { Xoffset, 0.0f, Zoffset, 1.0f}, { 0.0f, 1.0f, 0.0f } };

		V.push_back(v1);
	}

	pOutModel[0]->ModelMesh->Vertices = V;
	pOutModel[0]->ModelMesh->Indices = I;

	pOutModel[0]->ModelMaterial->CompilePass(Device, "RenderScreen.hlsl", "DrawBuffer", (PASSTYPE)(PASSTYPE_VERTEX | PASSTYPE_FRAGMENT));
}