#pragma once

#ifdef _INDEX16
typedef WORD Index;
#endif
#ifdef  _INDEX32
typedef UINT Index;
#endif


using namespace std;

struct UVVertex
{
	XMFLOAT4 Position;
	XMFLOAT2 UV;
};


struct Vertex
{
	XMFLOAT4 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};

template<class _VertTy>
struct Mesh
{
	vector<_VertTy> Vertices;
	vector<Index> Indices;
	UINT TriangleCount;
};

template<class _VertTy>
struct Triangle
{
	_VertTy Tri[3];
};

template<class _VertTy>
struct Model
{
	Mesh<_VertTy>* ModelMesh;
	MeshBuffer* ModelBuffer;


	Material* ModelMaterial;

	void operator=(WaveFrontReader<Index> OBJModel)
	{
		ModelMesh = new Mesh<_VertTy>();
		ModelMesh->Vertices.reserve(145209);
		Vertex V;
		for (int i = 0; i < OBJModel.vertices.size(); i++)
		{

			V.Position = XMFLOAT4(OBJModel.vertices[i].position.x, OBJModel.vertices[i].position.y, OBJModel.vertices[i].position.z, 1.0f);
			V.Normal = OBJModel.vertices[i].normal;
			V.UV = OBJModel.vertices[i].textureCoordinate;
		
			ModelMesh->Vertices.push_back(V);
		}
		ModelMesh->Indices = OBJModel.indices;
		ModelBuffer = new MeshBuffer();
		ModelMaterial = new Material();
		
	}
};

template<class _VertTy>
struct ModelArray
{
	Mesh<_VertTy>** ModelMeshes;
	MeshBuffer** ModelBuffers;
	
	Material** ModelMaterials;
};

struct Transform
{
	XMFLOAT4X4 World;
	XMFLOAT4X4 View;
	XMFLOAT4X4 Projection;
};

struct ViewProject
{
	XMFLOAT4X4 View;
	XMFLOAT4X4 Projection;
};

//----------------------------//

