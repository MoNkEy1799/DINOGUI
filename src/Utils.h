#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <array>
#include <string>
#include <iostream>
#include <functional>

#define DINOCOLOR_WINDOW_DARK DINOGUI::Color{ 0.1412f, 0.1451f, 0.1804f }
#define DINOCOLOR_WINDOW_LIGHT DINOGUI::Color{ 0.9412f, 0.9412f, 0.9412f }

#define DINOCOLOR_LIGHTGRAY DINOGUI::Color{ 0.7f, 0.7f, 0.7f, 1.0f }


#define DINOGUI_THEME_DARK \
{ \
	DINOCOLOR_LIGHTGRAY, DINOGUI::Color{ 0.3f, 0.3f, 0.3f }, DINOGUI::Color{ 0.1f, 0.1f, 0.1f }, \
	DINOCOLOR_LIGHTGRAY, DINOGUI::Color{ 0.99f, 0.4f, 0.4f }, DINOGUI::Color{ 0.1f, 0.1f, 0.1f }, \
	DINOCOLOR_LIGHTGRAY, DINOGUI::Color{ 0.3f, 0.3f, 0.3f }, DINOGUI::Color{ 0.1f, 0.1f, 0.1f }, \
}

#define DINOGUI_FONT_DEFAULT DINOGUI::Font{ 12.0f, "Segoe UI", DINOGUI::FontWeight::NORMAL }

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
	float r, g, b, a;
	static D2D1_COLOR_F d2d1(Color c);
};

struct ColorTheme
{
	Color txt, bg, brd;
	Color txt_h, bg_h, brd_h;
	Color txt_c, bg_c, brd_c;
};

#define DINOGUI_THEME_LIGHT ColorTheme \
{ \
	Color{ 0.0000f, 0.0000f, 0.0000f }, Color{ 0.8824f, 0.8824f, 0.8824f }, Color{ 0.6784f, 0.6784f, 0.6784f }, \
	Color{ 0.0000f, 0.0000f, 0.0000f }, Color{ 0.8980f, 0.9451f, 0.9843f }, Color{ 0.0000f, 0.4706f, 0.8431f }, \
	Color{ 0.0000f, 0.0000f, 0.0000f }, Color{ 0.8000f, 0.8941f, 0.9686f }, Color{ 0.0039f, 0.3373f, 0.6078f } \
}

struct DPIHandler
{
	static void Initialize(HWND windowHandle);
	static float PixelsToDips(float f);
	static D2D1_POINT_2F adjusted(D2D1_POINT_2F point);
	static D2D1_RECT_F adjusted(D2D1_RECT_F rect);

private:
    static float m_scale;
	static float DpiAdjusted(float f);
};

struct Timer
{
	Timer(HWND windowHandle, uint32_t timeout = 1000, std::function<void()> callback = nullptr);
	void start();
	void stop();
	void restart();
	bool isActive();

	uint32_t timeoutDelay;
	std::function<void()> callback;

private:
	static void timerFunction(HWND, uint32_t, uint64_t classPtr, DWORD);
	
	bool m_active;
	HWND m_windowHandle;
};

enum class EventType { ENTER_EVENT, LEAVE_EVENT, CLICK_EVENT, REALEASE_EVENT, UNSELECT_EVENT };

struct Event
{
	Event(EventType type, float mouseX, float mouseY) : type(type), mouseX(mouseX), mouseY(mouseY) {};
	EventType type;
	float mouseX, mouseY;
};

// TODO: maybe create enum for possible color depths
// template<enum class T, size_t S>
template<typename T, size_t S>
struct PixelBuffer
{
	PixelBuffer(uint32_t width, uint32_t height) : m_width(width), m_height(height) {};
	void fillBuffer(T value)
	{

	}
	void insertPixel(T value, size_t position)
	{

	}

private:
	uint32_t m_width, m_height;
	std::array<T, S> m_buffer;

	static T invertByteOrder(T value)
	{

	}
};

D2D1_COLOR_F toD2DColorF(const DINOGUI::Color& color);
std::wstring toWideString(const std::string& string);
void throwIfFailed(HRESULT result, const std::string& message = "");
void throwIfFailed(bool result, const std::string& message = "");

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
