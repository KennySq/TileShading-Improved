#pragma once
class InputManager
{
private:
	IDirectInput8* Input = nullptr;
	IDirectInputDevice8* Keyboard = nullptr;
	IDirectInputDevice8* Mouse = nullptr;

	Camera* MainCamera;

	unsigned char KeyboardState[256];
	DIMOUSESTATE MouseState;

	float MouseX = 0;
	float MouseY = 0;

	int Width;
	int Height;

public:
	HRESULT ReadKey();
	HRESULT ReadMouse();

	_inline void GetMousePosition(int& X, int& Y)
	{
		X = MouseX;
		Y = MouseY;
	}

	HRESULT Intialize(HINSTANCE Inst, Camera* Cam, int Width, int Height);
	void Update(float Delta);
	void Render(float Delta);
	void Release();

};

