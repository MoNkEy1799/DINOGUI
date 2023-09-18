#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <array>
#include <string>
#include <iostream>
#include <functional>
#include <random>

#define DINOCOLOR_WINDOW_DARK DINOGUI::Color{ 36, 37, 46 }
#define DINOCOLOR_WINDOW_LIGHT DINOGUI::Color{ 240, 240, 240 }

#define DINOCOLOR_WHITE DINOGUI::Color{ 255, 255, 255 }
#define DINOCOLOR_LIGHTGRAY DINOGUI::Color{ 178, 178, 178 }
#define DINOCOLOR_GRAY DINOGUI::Color{ 128, 128, 128 }
#define DINOCOLOR_DARKGRAY DINOGUI::Color{ 98, 98, 98 }
#define DINOCOLOR_BLACK DINOGUI::Color{ 0, 0, 0 }
#define DINOCOLOR_RED DINOGUI::Color{ 255, 0, 0 }
#define DINOCOLOR_GREEN DINOGUI::Color{ 0, 255, 0}
#define DINOCOLOR_BLUE DINOGUI::Color{ 0, 0, 255 }


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

class Core;

template<typename T>
struct Point
{
	T x, y;
};

template<typename T>
struct Size
{
	T width, height;
};

template<typename T>
struct Rect
{
	T left, top, right, bottom;
};

struct ResizeState
{
	const Size<float>* size;
	const Size<float>* minSize;
	const Size<float>* maxSize;
	bool stretchWidth, stretchHeight;
};

struct Color
{
	int r, g, b, a = 255;
	static D2D1_COLOR_F d2d1(Color c);
};

enum class WidgetType;
struct ColorTheme
{
	std::array<Color, 7> background;
	std::array<Color, 7> border;
	std::array<Color, 7> text;
	std::array<Color, 7> background2;
	std::array<Color, 7> border2;
	std::array<Color, 7> text2;
	std::array<Color, 7> addColor;
	float width, width2, width3;

	static void createDefault(ColorTheme* theme, WidgetType type, const Color& accentColor = DINOCOLOR_BLUE, bool dark = false);
	static void createFromFile(ColorTheme* theme, WidgetType type, const std::string& filename);
	static void createFromString(ColorTheme* theme, WidgetType type, const std::string& string);
};

enum class FontWeight
{
	XLIGHT = DWRITE_FONT_WEIGHT_THIN,
	LIGHT = DWRITE_FONT_WEIGHT_LIGHT,
	NORMAL = DWRITE_FONT_WEIGHT_NORMAL,
	BOLD = DWRITE_FONT_WEIGHT_BOLD,
	XBOLD = DWRITE_FONT_WEIGHT_HEAVY
};

enum class FontStyle
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

enum class Alignment
{
	LEFT,
	RIGHT,
	TOP,
	BOTTOM,
	CENTER
};

static const std::array<int, 5> DWrite_Alignment_Map =
{
	DWRITE_TEXT_ALIGNMENT_LEADING,
	DWRITE_TEXT_ALIGNMENT_TRAILING,
	DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
	DWRITE_PARAGRAPH_ALIGNMENT_FAR,
	DWRITE_TEXT_ALIGNMENT_CENTER
};

class Text
{
public:
	Text(Core* core, const std::string& text);
	~Text();

	void draw(D2D1_RECT_F rectangle, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush);
	void setText(const std::string& text);
	void setFont(const Font& font);
	void setColor(const Color& color);
	void unsetColor();
	void setAlignment(Alignment align);

	std::string& getText();
	IDWriteTextFormat* getFontFormat();
	bool fontFormatChanged;

private:
	IDWriteTextFormat* m_fontFormat;
	Core* m_core;
	std::string m_text;
	Font m_font;
	DWRITE_TEXT_ALIGNMENT m_hAlign;
	DWRITE_PARAGRAPH_ALIGNMENT m_vAlign;
	Color m_color;
	bool m_colorSet;

	bool createFontFormat();
};

class DPIHandler
{
public:
	static void Initialize(HWND windowHandle);
	static float PixelsToDips(float f);
	static D2D1_POINT_2F adjusted(D2D1_POINT_2F point);
	static D2D1_RECT_F adjusted(D2D1_RECT_F rect);
	static float adjusted(float f);
	static float getScale();

private:
    static float m_scale;
	static float adjust(float f, float dir = 0.5f);
};

class Timer
{
public:
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

class Random
{
public:
	static void seed(uint32_t seed = std::random_device()());
	static std::mt19937 getEngine();

	static float randFloat(float min = 0.0f, float max = 1.0f);
	static int randInt(int min, int max);
	static bool randBool();

private:
	static std::mt19937 m_randomEngine;
};

enum class EventType { ENTER_EVENT, LEAVE_EVENT, CLICK_EVENT, REALEASE_EVENT, UNSELECT_EVENT, HOLD_EVENT };

struct Event
{
	Event(EventType type, float mouseX, float mouseY) : type(type), mouseX(mouseX), mouseY(mouseY) {};
	EventType type;
	float mouseX, mouseY;
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
