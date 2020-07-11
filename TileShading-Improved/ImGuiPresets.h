#pragma once

static void SetLightProperties(Light& InLight, float* Position, float* Color, int Index)
{
	ImGui::Begin((string("Light " + Index) ).c_str());
	ImGui::SliderFloat3("Position", Position, -50.0f, 50.0f);

	ImGui::SliderFloat3("Color", Color, 0.0f, 1.0f);
	ImGui::SliderFloat("Intensity", &Position[3], 0.0f, 10.0f);
	ImGui::SliderFloat("Radius", &Color[3], 0.01f, 10.0f);

	InLight.Position = XMFLOAT4(Position[0], Position[1], Position[2], Position[3]);
	InLight.Color = XMFLOAT4(Color[0], Color[1], Color[2], Color[3]);

	ImGui::End();
}

static void UpdateLightSources(ID3D11DeviceContext* Context, Light* Lights, ID3D11Buffer* LightBuffer)
{
	Context->UpdateSubresource(LightBuffer, 0, nullptr, Lights, 0, 0);
}