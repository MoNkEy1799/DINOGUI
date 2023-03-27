#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <array>
#include <string>
#include <iostream>

#define DINOGUI_STYLE_DARK \
{ \
	D2D1::ColorF(0.8f, 0.8f, 0.8f), D2D1::ColorF(0.3f, 0.3f, 0.3f), D2D1::ColorF(0.1f, 0.1f, 0.1f), \
	D2D1::ColorF(0.8f, 0.8f, 0.8f), D2D1::ColorF(0.99f, 0.4f, 0.4f), D2D1::ColorF(0.1f, 0.1f, 0.1f), \
	D2D1::ColorF(0.8f, 0.8f, 0.8f), D2D1::ColorF(0.3f, 0.3f, 0.3f), D2D1::ColorF(0.1f, 0.1f, 0.1f), \
	12.0f, "Segoe UI", DWRITE_FONT_WEIGHT_NORMAL \
}

namespace DINOGUI
{

struct DPIConverter
{
    static void Initialize(HWND windowHandle)
    {
        uint32_t dpi = GetDpiForWindow(windowHandle);
        scale = dpi / 96.0f;
    }

    template <typename T>
    static float PixelsToDips(T x)
    {
        return static_cast<float>(x) / scale;
    }

private:
    static float scale;
};

struct Style
{
	D2D1_COLOR_F text, background, border;
	D2D1_COLOR_F text_Hover, background_Hover, border_Hover;
	D2D1_COLOR_F text_Click, background_Click, border_Click;
	float fontSize;
	std::string fontFamily;
	DWRITE_FONT_WEIGHT fontWeight;
};

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
		windowClass.lpszClassName = L"BaseWindow";
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

		RegisterClass(&windowClass);

		m_windowHandle = CreateWindowEx(0, L"BaseWindow", windowName, windowStyle, x, y, width, height, 0, 0, GetModuleHandle(0), this);

		return (m_windowHandle ? true : false);
	}

	HWND getWindowHandle() { return m_windowHandle; };

protected:
	virtual LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam) = 0;

	HWND m_windowHandle;
};

}
