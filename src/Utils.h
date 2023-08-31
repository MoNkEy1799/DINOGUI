#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <array>
#include <string>
#include <iostream>
#include <functional>

#define DINOCOLOR_WINDOW_DARK DINOGUI::Color{ 36, 37, 46 }
#define DINOCOLOR_WINDOW_LIGHT DINOGUI::Color{ 240, 240, 240 }

#define DINOCOLOR_LIGHTGRAY DINOGUI::Color{ 178, 178, 178 }


#define DINOGUI_THEME_DARK \
{ \
	DINOCOLOR_LIGHTGRAY, DINOGUI::Color{ 76, 76, 76 }, DINOGUI::Color{ 25, 25, 25 }, \
	DINOCOLOR_LIGHTGRAY, DINOGUI::Color{ 253, 102, 102 }, DINOGUI::Color{ 25, 25, 25 }, \
	DINOCOLOR_LIGHTGRAY, DINOGUI::Color{ 76, 76, 76 }, DINOGUI::Color{ 25, 25, 25 }, \
}

#define DINOGUI_FONT_DEFAULT DINOGUI::Font{ 12.0f, "Segoe UI", DINOGUI::FontWeight::NORMAL, DINOGUI::FontStyle::NORMAL }

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

enum FontStyle
{
	NORMAL = DWRITE_FONT_STYLE_NORMAL,
	ITALIC = DWRITE_FONT_STYLE_ITALIC,
	OBLIQUE = DWRITE_FONT_STYLE_OBLIQUE
};

struct Font
{
	Font(float size, const std::string& family, FontWeight weight, FontStyle style)
		: size(size), family(family), weight(weight), style(style) {};
	float size;
	std::string family;
	FontWeight weight;
	FontStyle style;
};

enum H_TextAlignment
{
	LEADING = DWRITE_TEXT_ALIGNMENT_LEADING,
	CENTER = DWRITE_TEXT_ALIGNMENT_CENTER,
	TRAILING = DWRITE_TEXT_ALIGNMENT_TRAILING,
	JUSTIFIED = DWRITE_TEXT_ALIGNMENT_JUSTIFIED
};

enum V_TextAlignment
{
	TOP = DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
	CENTER = DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
	BOTTOM = DWRITE_PARAGRAPH_ALIGNMENT_FAR
};

struct Text
{
	Text(const std::string& text, const Font& font = DINOGUI_FONT_DEFAULT);
	~Text();

	Font font;
	H_TextAlignment horizontalAlignment;
	V_TextAlignment verticalAlignment;
};

struct Color
{
	int r, g, b, a = 255;
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
	Color{ 0, 0, 0 }, Color{ 225, 225, 225 }, Color{ 173, 173, 173 }, \
	Color{ 0, 0, 0 }, Color{ 229, 241, 251 }, Color{ 0, 120, 215 }, \
	Color{ 0, 0, 0 }, Color{ 204, 228, 247 }, Color{ 1, 86, 155 } \
}

struct DPIHandler
{
	static void Initialize(HWND windowHandle);
	static float PixelsToDips(float f);
	static D2D1_POINT_2F adjusted(D2D1_POINT_2F point);
	static D2D1_RECT_F adjusted(D2D1_RECT_F rect);
	static float getScale();

private:
    static float m_scale;
	static float DpiAdjusted(float f, float dir = 0.5f);
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

struct Point
{
	float x, y;
};

template<typename T>
struct GridEntry
{
	T entry;
	int rowSpan, colSpan;
};

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
