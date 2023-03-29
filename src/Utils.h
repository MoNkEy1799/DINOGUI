#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <array>
#include <string>
#include <iostream>

#define DINOCOLOR_WINDOW_DARK DINOGUI::Color(0.1412f, 0.1451f, 0.1804f)
#define DINOCOLOR_WINDOW_LIGHT DINOGUI::Color(0.9412f, 0.9412f, 0.9412f)

#define DINOCOLOR_LIGHTGRAY DINOGUI::Color(0.7f, 0.7f, 0.7f, 1.0f)


#define DINOGUI_THEME_DARK \
{ \
	DINOCOLOR_LIGHTGRAY, DINOGUI::Color(0.3f, 0.3f, 0.3f), DINOGUI::Color(0.1f, 0.1f, 0.1f), \
	DINOCOLOR_LIGHTGRAY, DINOGUI::Color(0.99f, 0.4f, 0.4f), DINOGUI::Color(0.1f, 0.1f, 0.1f), \
	DINOCOLOR_LIGHTGRAY, DINOGUI::Color(0.3f, 0.3f, 0.3f), DINOGUI::Color(0.1f, 0.1f, 0.1f), \
}

#define DINOGUI_THEME_LIGHT \
{ \
	DINOGUI::Color(0.0000f, 0.0000f, 0.0000f), DINOGUI::Color(0.8824f, 0.8824f, 0.8824f), DINOGUI::Color(0.6784f, 0.6784f, 0.6784f), \
	DINOGUI::Color(0.0000f, 0.0000f, 0.0000f), DINOGUI::Color(0.8980f, 0.9451f, 0.9843f), DINOGUI::Color(0.0000f, 0.4706f, 0.8431f), \
	DINOGUI::Color(0.0000f, 0.0000f, 0.0000f), DINOGUI::Color(0.8000f, 0.8941f, 0.9686f), DINOGUI::Color(0.0039f, 0.3373f, 0.6078f), \
}

#define DINOGUI_FONT_DEFAULT DINOGUI::Font{ 12.0f, "Segeo UI", DINOGUI::FontWeight::NORMAL }

#define DINOGUI_ALL_MOUSE_BUTTONS MK_LBUTTON + MK_MBUTTON + MK_RBUTTON + MK_XBUTTON1 + MK_XBUTTON2

namespace DINOGUI
{

enum FontWeight
{
	XLIGHT = DWRITE_FONT_WEIGHT_THIN,
	LIGHT = DWRITE_FONT_WEIGHT_LIGHT,
	NORMAL = DWRITE_FONT_WEIGHT_NORMAL,
	BOLD = DWRITE_FONT_WEIGHT_BOLD,
	XBOLD = DWRITE_FONT_WEIGHT_HEAVY
};

struct Font
{
	Font(float fontSize, const std::string& fontFamily, FontWeight fontWeight) : size(fontSize), family(fontFamily), weight(fontWeight) {};
	float size;
	std::string family;
	FontWeight weight;
};

struct Color
{
	Color(float red, float green, float blue) : r(red), g(green), b(blue), a(1.0f) {};
	Color(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) {};
	float r, g, b, a;
};

struct ColorTheme
{
	Color txt, bg, brd;
	Color txt_h, bg_h, brd_h;
	Color txt_c, bg_c, brd_c;
};

struct DPIConverter
{
	static void Initialize(HWND windowHandle);
	static float PixelsToDips(float x);

private:
    static float scale;
};

D2D1_COLOR_F toD2DColorF(const DINOGUI::Color& color);
std::wstring toWideString(const std::string& string);

template<class C>
void safeReleaseInterface(C** pointerToInterface)
{
	if (*pointerToInterface)
	{
		(*pointerToInterface)->Release();
		*pointerToInterface = nullptr;
	}
}

template<class CLASS_TYPE>
class TemplateWindow
{
public:
	TemplateWindow() : m_windowHandle(nullptr) { }

	static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT messageCode, WPARAM wParam, LPARAM lParam)
	{
		CLASS_TYPE* mainThis = nullptr;

		if (messageCode == WM_NCCREATE)
		{
			CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
			mainThis = (CLASS_TYPE*)createStruct->lpCreateParams;
			SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR)mainThis);

			mainThis->m_windowHandle = windowHandle;
		}
		else
		{
			mainThis = (CLASS_TYPE*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);
		}

		if (mainThis)
		{
			return mainThis->HandleMessage(messageCode, wParam, lParam);
		}
		else
		{
			return DefWindowProc(windowHandle, messageCode, wParam, lParam);
		}
	}

	bool createWindow(PCWSTR windowName, DWORD windowStyle, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT)
	{
		WNDCLASS windowClass = { 0 };

		windowClass.lpfnWndProc = CLASS_TYPE::WindowProc;
		windowClass.hInstance = GetModuleHandle(0);
		windowClass.lpszClassName = L"DINOGUI_CORE_WINDOW";
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

		RegisterClass(&windowClass);

		m_windowHandle = CreateWindowEx(0, L"DINOGUI_CORE_WINDOW", windowName, windowStyle, x, y, width, height, 0, 0, GetModuleHandle(0), this);

		return (m_windowHandle ? true : false);
	}

	HWND getWindowHandle() { return m_windowHandle; };

protected:
	virtual LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam) = 0;

	HWND m_windowHandle;
};

}
