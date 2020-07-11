#include "stdafx.h"
#include "InputManager.h"

HRESULT InputManager::ReadKey()
{
	HRESULT Result = Keyboard->GetDeviceState(256, KeyboardState);
	assert(Result == S_OK && "Failed to get keyboard state.");

	if (KeyboardState[DIK_W] & 0x80)
	{
		MainCamera->Moveable(0.01f, 0.0f, 0.0f);
	}
	if (KeyboardState[DIK_A] & 0x80)
	{
		MainCamera->Moveable(0.0f, 0.0f, 0.01f);
	}

	if (KeyboardState[DIK_D] & 0x80)
	{
		MainCamera->Moveable(0.0f, 0.0f, -0.01f);
	}

	if (KeyboardState[DIK_S] & 0x80)
	{
		MainCamera->Moveable(-0.01f, 0.0f, 0.0f);

	}

	if (KeyboardState[DIK_Q] & 0x80)
	{
		MainCamera->Moveable(0.0f, 0.01f, 0.0f);

	}
	if (KeyboardState[DIK_E] & 0x80)
	{
		MainCamera->Moveable(0.0f, -0.01f, 0.0f);
	}

	return S_OK;
}

HRESULT InputManager::ReadMouse()
{
	HRESULT Result = Mouse->GetDeviceState(sizeof(DIMOUSESTATE), &MouseState);
	if (FAILED(Result))
		return E_FAIL;
	


	MouseX += MouseState.lX;
	MouseY += MouseState.lY;

	if (MouseX < 0) MouseX = 0;
	if (MouseY < 0) MouseY = 0;

	if (MouseX > Width) MouseX = Width;
	if (MouseY > Height) MouseY = Height;

	if (MouseState.rgbButtons[1] & 0x80)
		MainCamera->Rotate(MouseState.lX*0.001f, MouseState.lY * 0.001f, 0.0f, true);

	return S_OK;
}

HRESULT InputManager::Intialize(HINSTANCE Inst, Camera* Cam, int Width, int Height)
{
	if (FAILED(DirectInput8Create(Inst, DIRECTINPUT_HEADER_VERSION, IID_IDirectInput8, (void**)&Input, nullptr)))
		return E_FAIL;

	if (FAILED(Input->CreateDevice(GUID_SysKeyboard, &Keyboard, nullptr)))
		return E_FAIL;

	if (FAILED(Keyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;

	if (FAILED(Keyboard->Acquire()))
		return E_FAIL;

	if (FAILED(Input->CreateDevice(GUID_SysMouse, &Mouse, NULL)))
		return E_FAIL;

	if (FAILED(Mouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	if (FAILED(Mouse->Acquire()))
		return E_FAIL;

	MainCamera = Cam;

	return S_OK;
}

void InputManager::Update(float Delta)
{
	ReadKey();
	ReadMouse();
}
