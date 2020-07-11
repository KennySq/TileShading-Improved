#pragma once

//typedef std::map<UINT, Instance*> InstanceCache;
typedef std::map<UINT, MeshBuffer*> MeshBufferCache;
typedef std::map<string, Material*> MaterialCache;
class Scene
{
	Camera* MainCamera = nullptr;

	//InstanceCache SceneInstances;
	MaterialCache SceneMaterials;
	MeshBufferCache SceneMeshBuffers;

	UINT InstanceID = 0;
	UINT MeshBufferID = 0;

public:
	_inline Scene() { MainCamera = Camera::MakePerspectiveCamera(XM_PIDIV2, 1.777f, 0.01f, 1000.0f); }
	//InstanceCache& GetInstances() { return SceneInstances; }
	MaterialCache& GetMaterials() { return SceneMaterials; }
	MeshBufferCache& GetMeshBuffers() { return SceneMeshBuffers; }

	//void AddInstance(Instance& Inst) { assert(&Inst); SceneInstances.insert_or_assign(InstanceID++, &Inst); }
	void AddMeshBuffer(MeshBuffer& Buffer) { assert(&Buffer);  SceneMeshBuffers.insert_or_assign(MeshBufferID++, &Buffer); }
	void AddMaterial(string Name, Material* Mat) { assert(Mat); SceneMaterials.insert_or_assign(Name, Mat); }

	_inline Camera* GetMainCamera() { return MainCamera; }
};
