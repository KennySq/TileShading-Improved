#pragma once

using namespace std;

enum DSType
{
	BRDF = 0,
	BTDF,
	BSDF,
	BSSRDF,
};

enum PASSTYPE
{
	PASSTYPE_VERTEX = 1,
	PASSTYPE_FRAGMENT = 2,
	PASSTYPE_GEOMETRY = 4,
	PASSTYPE_COMPUTE = 8,
};

struct Pass
{
	PASSTYPE TypeFlag;
	ID3D11VertexShader* VS = nullptr;
	ID3D11PixelShader* PS = nullptr;
	ID3D11GeometryShader* GS = nullptr;
	ID3D11ComputeShader* CS = nullptr;

	ID3D11InputLayout* IL = nullptr;
	ID3D11ShaderReflection* Reflect = nullptr;


	Pass() {};
	~Pass() {
		if (VS)
			VS->Release();
		if (PS)
			PS->Release();
		if (GS)
			GS->Release();
		if (IL)
			IL->Release();
		if (Reflect)
			Reflect->Release();
	
	};
};

class Material
{
	DSType Type;
	map<string, Pass*> Passes;
public:

	HRESULT CompilePass(ID3D11Device* Device, string Path, string PassName, PASSTYPE Flag);
	HRESULT OverridePass(ID3D11Device* Device, string OldPass, string Path, string PassName, PASSTYPE Flag);

	HRESULT ReflectShader(ID3D11Device* Device, ID3DBlob* VBlob, ID3D11InputLayout** pOutInputLayout, ID3D11ShaderReflection** pOutReflection);


	//template<class _Ty>
	//_Ty* RequestInterface(string PassName) {
	//	_Ty* P = &Passes[PassName];
	//
	//	
	//	return (_Ty*)(P);
	//
	//
	//}
	
	_inline Pass* GetPass(string PassName) { return Passes[PassName]; }
	//_inline ID3D11InputLayout* GetInputLayout(string PassName) { return Passes[PassName]->IL; }
};

