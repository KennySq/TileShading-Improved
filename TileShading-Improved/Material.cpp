#include "stdafx.h"
#include "Material.h"

HRESULT Material::CompilePass(ID3D11Device* Device, string Path, string PassName, PASSTYPE Flag)
{
	HRESULT Result;
	DWORD CompileFlag = 0;
	ID3DBlob* VBlob = nullptr, *PBlob = nullptr ,*GBlob = nullptr,*CBlob = nullptr,* ErrBlob = nullptr;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D11GeometryShader* GS;
	ID3D11ComputeShader* CS;
	ID3D11InputLayout* IL;
	ID3D11ShaderReflection* SR;
	Pass* P = nullptr;

	USES_CONVERSION;
	
	if(P == nullptr)
		P = new Pass();

#ifdef _DEBUG
	CompileFlag |= D3DCOMPILE_DEBUG;
#endif

	if (Flag & PASSTYPE_VERTEX)
	{
		auto Pass = (PassName + "VS");
		Result = D3DCompileFromFile(A2W(Path.c_str()), nullptr, nullptr, Pass.c_str(), "vs_5_0", 0, CompileFlag, &VBlob, &ErrBlob);
		assert(Result == S_OK && "Failed to compile vertex shader");
		Result = Device->CreateVertexShader(VBlob->GetBufferPointer(), VBlob->GetBufferSize(), nullptr, &VS);
		assert(Result == S_OK && "Failed to create vertex shader");
		ReflectShader(Device, VBlob, &IL, &SR);
		P->VS = VS;
		P->IL = IL;
		P->Reflect = SR;


	}

	if (Flag & PASSTYPE_FRAGMENT)
	{
		Result = D3DCompileFromFile(A2W(Path.c_str()), nullptr, nullptr, (PassName + "PS").c_str(), "ps_5_0", 0, CompileFlag, &PBlob, &ErrBlob);
		assert(Result == S_OK && "Failed to compile pixel shader");
		Result = Device->CreatePixelShader(PBlob->GetBufferPointer(), PBlob->GetBufferSize(), nullptr, &PS);
		assert(Result == S_OK && "Failed to create pixel shader");
		P->PS = PS;

	}

	if (Flag & PASSTYPE_GEOMETRY)
	{
		
		Result = D3DCompileFromFile(A2W(Path.c_str()), nullptr, nullptr, (PassName + "GS").c_str(), "gs_5_0", 0, CompileFlag, &GBlob, &ErrBlob);
		assert(Result == S_OK && "Failed to compile geometry shader");
		Result = Device->CreateGeometryShader(GBlob->GetBufferPointer(), GBlob->GetBufferSize(), nullptr, &GS);
		assert(Result == S_OK && "Failed to create geometry shader.");
		P->GS = GS;

	}

	if (Flag & PASSTYPE_COMPUTE)
	{
		
		Result = D3DCompileFromFile(A2W(Path.c_str()), nullptr, nullptr, (PassName + "CS").c_str(), "cs_5_0", 0, CompileFlag, &CBlob, &ErrBlob);
		assert(Result == S_OK && "Failed to compile compute shader");
		Result = Device->CreateComputeShader(CBlob->GetBufferPointer(), CBlob->GetBufferSize(), nullptr, &CS);
		assert(Result == S_OK && "Failed to create compute shader.");
		P->CS = CS;
	}




	Passes.insert_or_assign(PassName, P);


	return S_OK;
}

HRESULT Material::OverridePass(ID3D11Device* Device, string OldPass, string Path, string PassName, PASSTYPE Flag)
{

	assert(Passes[OldPass] != nullptr && "There is no such pass.");
		
	Passes[OldPass]->~Pass();
	Passes.erase(OldPass);

	CompilePass(Device, Path, PassName, Flag);

	return S_OK;
}

HRESULT Material::ReflectShader(ID3D11Device* Device, ID3DBlob* VBlob,ID3D11InputLayout** pOutInputLayout, ID3D11ShaderReflection** pOutReflection)
{
	HRESULT Result;

	Result = D3DReflect(VBlob->GetBufferPointer(), VBlob->GetBufferSize(), __uuidof(ID3D11ShaderReflection), (void**)pOutReflection);
	assert(Result == S_OK && "Failed to reflect vertex shader.");

	unsigned int ByteOffset = 0;
	D3D11_SHADER_DESC ShaderDesc{};
	vector<D3D11_INPUT_ELEMENT_DESC> InputElements;
	D3D11_SIGNATURE_PARAMETER_DESC SignParam;
	pOutReflection[0]->GetDesc(&ShaderDesc);
	for (int i = 0; i < ShaderDesc.InputParameters; i++)
	{
		pOutReflection[0]->GetInputParameterDesc(i, &SignParam);

		D3D11_INPUT_ELEMENT_DESC Element;

		Element.SemanticName = SignParam.SemanticName;
		Element.SemanticIndex = SignParam.SemanticIndex;
		Element.InputSlot = 0;
		Element.AlignedByteOffset = ByteOffset;
		Element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		Element.InstanceDataStepRate = 0;

		if (SignParam.Mask == 1)
		{
			if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_UINT32) Element.Format = DXGI_FORMAT_R32_UINT;
			else if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_SINT32) Element.Format = DXGI_FORMAT_R32_SINT;
			else if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) Element.Format = DXGI_FORMAT_R32_FLOAT;

			ByteOffset += 4;
		}
		else if (SignParam.Mask <= 3)
		{
			if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_UINT32) Element.Format = DXGI_FORMAT_R32G32_UINT;
			else if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_SINT32) Element.Format = DXGI_FORMAT_R32G32_SINT;
			else if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) Element.Format = DXGI_FORMAT_R32G32_FLOAT;
			ByteOffset += 8;
		}
		else if (SignParam.Mask <= 7)
		{
			if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_UINT32) Element.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_SINT32) Element.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) Element.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			ByteOffset += 12;
		}

		else if (SignParam.Mask <= 15)
		{
			if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_UINT32) Element.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_SINT32) Element.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (SignParam.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) Element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			ByteOffset += 16;
		}


		InputElements.push_back(Element);
	}
	
	Result = Device->CreateInputLayout(InputElements.data(), InputElements.size(), VBlob->GetBufferPointer(), VBlob->GetBufferSize(), pOutInputLayout);
	assert(Result == S_OK && "Failed to create a input layout.");

	return S_OK;
}