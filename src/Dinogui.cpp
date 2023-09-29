#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <cmath>
#include <functional>
#include <string>
#include <random>

using namespace DINOGUI;

ResizeState::ResizeState(const Size<float>& size, const Size<float>& minSize, const Size<float>& maxSize)
	: size(size), minSize(minSize), maxSize(maxSize)
{
}

Text::Text(Core* core, const std::string& text)
	: m_font(DINOGUI_FONT_DEFAULT), m_color({ 0, 0, 0 }), m_fontFormat(nullptr),
	  m_core(core), fontFormatChanged(false), m_text(text), m_colorSet(false),
	  m_hAlign(DWRITE_TEXT_ALIGNMENT_CENTER), m_vAlign(DWRITE_PARAGRAPH_ALIGNMENT_CENTER)
{
}

Text::~Text()
{
	safeReleaseInterface(&m_fontFormat);
}

void Text::draw(D2D1_RECT_F rectangle, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
	if (!m_fontFormat)
	{
		throwIfFailed(createFontFormat(), "Failed to create text format");
		fontFormatChanged = true;
	}
	rectangle.left += 2.0f;
	rectangle.top += 2.0f;
	rectangle.right -= 2.0f;
	rectangle.bottom -= 2.0f;
	if (m_colorSet)
	{
		brush->SetColor(Color::d2d1(m_color));
	}
	renderTarget->DrawText(toWideString(m_text).c_str(), (uint32_t)m_text.size(), m_fontFormat, rectangle, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void Text::setText(const std::string& text)
{
	m_text = text;
}

void Text::setFont(const Font& font)
{
	m_font = font;
	safeReleaseInterface(&m_fontFormat);
}

void Text::setColor(const Color& color)
{
	m_color = color;
	m_colorSet = true;
}

void Text::unsetColor()
{
	m_colorSet = false;
}

void Text::setAlignment(Alignment align)
{
	switch (align)
	{
	case Alignment::LEFT:
	case Alignment::RIGHT:
		m_hAlign = (DWRITE_TEXT_ALIGNMENT)DWrite_Alignment_Map[(int)align];
		break;

	case Alignment::TOP:
	case Alignment::BOTTOM:
		m_vAlign = (DWRITE_PARAGRAPH_ALIGNMENT)DWrite_Alignment_Map[(int)align];
		break;

	case Alignment::CENTER:
		m_hAlign = (DWRITE_TEXT_ALIGNMENT)DWrite_Alignment_Map[(int)align];
		m_vAlign = (DWRITE_PARAGRAPH_ALIGNMENT)DWrite_Alignment_Map[(int)align];
		break;
	}
	safeReleaseInterface(&m_fontFormat);
}

std::string& Text::getText()
{
	return m_text;
}

IDWriteTextFormat* Text::getFontFormat()
{
	return m_fontFormat;
}

bool Text::createFontFormat()
{
	HRESULT hResult = getWriteFactory(m_core)->CreateTextFormat(
		toWideString(m_font.family).c_str(), NULL, (DWRITE_FONT_WEIGHT)m_font.weight,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, m_font.size, L"en-us", &m_fontFormat);

	if (FAILED(hResult))
	{
		return false;
	}
	if (!SUCCEEDED(m_fontFormat->SetTextAlignment((DWRITE_TEXT_ALIGNMENT)m_hAlign)))
	{
		return false;
	}
	if (!SUCCEEDED(m_fontFormat->SetParagraphAlignment((DWRITE_PARAGRAPH_ALIGNMENT)m_vAlign)))
	{
		return false;
	}
	if (!SUCCEEDED(m_fontFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP)))
	{
		return false;
	}
	return true;
}

D2D1_COLOR_F Color::d2d1(Color c)
{
	return D2D1::ColorF(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
}

void ColorTheme::createDefault(ColorTheme* theme, WidgetType type, const Color& accentColor, bool dark)
{
	theme->width = 1.0f;
	theme->width2 = 1.0f;
	theme->width3 = 1.0f;
	theme->background = {
			Color{ 225, 225, 225 },
			Color{ 229, 241, 251 },
			Color{ 204, 228, 247 },
			Color{ 229, 241, 251 },
			Color{ 204, 228, 247 },
			Color{ 204, 228, 247 },
			Color{ 204, 228, 247 } };
	theme->border = {
			Color{ 173, 173, 173 },
			Color{ 0, 120, 215 },
			Color{ 1, 86, 155 },
			Color{ 0, 120, 215 },
			Color{ 1, 86, 155 },
			Color{ 1, 86, 155 },
			Color{ 1, 86, 155 } };
	theme->text.fill(DINOCOLOR_BLACK);
	theme->addColor.fill(DINOCOLOR_BLACK);

	switch (type)
	{
	case WidgetType::NONE:
	case WidgetType::BUTTON:
	case WidgetType::LABEL:
	case WidgetType::CANVAS:
		theme->background2 = theme->background;
		theme->border2 = theme->border;
		theme->text2 = theme->text;
		break;

	case WidgetType::CHECKBOX:
		theme->background2.fill(DINOCOLOR_WHITE);
		theme->border2 = theme->border;
		theme->border2[0] = DINOCOLOR_DARKGRAY;
		theme->text2 = theme->text;
		break;

	case WidgetType::TEXTEDIT:
		theme->background.fill(DINOCOLOR_WHITE);
		theme->border[0] = DINOCOLOR_DARKGRAY;
		theme->border[1] = DINOCOLOR_BLACK;
		theme->background2.fill(Color{ 0, 120, 215 });
		theme->border2.fill(DINOCOLOR_BLACK);
		theme->text2.fill(DINOCOLOR_WHITE);
		theme->addColor.fill(DINOCOLOR_LIGHTGRAY);
		break;

	case WidgetType::IMAGE:
	case WidgetType::TABLE:
		theme->background.fill(DINOCOLOR_WHITE);
		theme->background2 = theme->background;
		theme->border2.fill(DINOCOLOR_LIGHTGRAY);
		theme->text2 = theme->text;
		break;

	case WidgetType::COMBOBOX:
		theme->background2.fill(Color{ 0, 120, 215 });
		theme->background2[3] = DINOCOLOR_DARKGRAY;
		theme->border2 = theme->border;
		theme->text2.fill(DINOCOLOR_WHITE);
		theme->addColor.fill(DINOCOLOR_WHITE);
		break;

	case WidgetType::SLIDER:
		theme->background[0] = DINOCOLOR_DARKGRAY;
		theme->background[1] = Color{ 0, 120, 215 };
		theme->background[2] = Color{ 1, 86, 155 };
		theme->background2.fill(DINOCOLOR_WINDOW_LIGHT);
		theme->border.fill(Color{ 173, 173, 173 });
		theme->border2 = theme->border;
		theme->text2 = theme->text;
		break;
	}
};

void ColorTheme::createFromFile(ColorTheme* theme, WidgetType type, const std::string& filename)
{

}

void ColorTheme::createFromString(ColorTheme* theme, WidgetType type, const std::string& string)
{

}

float DPIHandler::m_scale = 1.0f;

void DPIHandler::Initialize(HWND windowHandle)
{
    uint32_t dpi = GetDpiForWindow(windowHandle);
    m_scale = dpi / 96.0f;
}

float DPIHandler::PixelsToDips(float f)
{
	return f / m_scale;
}

D2D1_POINT_2F DPIHandler::adjusted(D2D1_POINT_2F point)
{
	return { adjust(point.x), adjust(point.y) };
}

D2D1_RECT_F DPIHandler::adjusted(D2D1_RECT_F rect)
{
	return { adjust(rect.left), adjust(rect.top),
			 adjust(rect.right, -0.5f), adjust(rect.bottom, -0.5f) };
}
float DPIHandler::adjusted(float f)
{
	return adjust(f);
}

float DPIHandler::getScale()
{
	return m_scale;
}

float DPIHandler::adjust(float f, float dir)
{
	return (std::floor(f * m_scale) + dir) / m_scale;
}

Timer::Timer(Core* core, uint32_t timeout, std::function<void()> callback)
	: timeoutDelay(timeout), callback(callback), m_active(false), m_core(core), m_windowHandle(nullptr)
{
	CoreInterface::addTimer(core, this);
	m_windowHandle = CoreInterface::getWindowHandle(m_core);
}

Timer::~Timer()
{
	CoreInterface::removeTimer(m_core, this);
}

void Timer::start()
{
	if (!callback)
	{
		return;
	}
	SetTimer(m_windowHandle, (uint64_t)this, timeoutDelay, Timer::timerFunction);
	m_active = true;
}

void Timer::stop()
{
	KillTimer(m_windowHandle, (uint64_t)this);
	m_active = false;
}

void Timer::restart()
{
	if (!m_active)
	{
		return;
	}
	stop();
	start();
}

bool Timer::isActive()
{
	return m_active;
}

void Timer::timerFunction(HWND, uint32_t, uint64_t classPtr, DWORD)
{
	Timer* self = (Timer*)classPtr;
	self->callback();
}

std::mt19937 Random::m_randomEngine;

void Random::seed(uint32_t seed)
{
	m_randomEngine.seed(seed);
}

std::mt19937 Random::getEngine()
{
	return m_randomEngine;
}

float Random::randFloat(float min, float max)
{
	std::uniform_real_distribution<float> floatDist(min, max);
	return floatDist(m_randomEngine);
}

int Random::randInt(int min, int max)
{
	std::uniform_int_distribution<int> intDist(min, max);
	return intDist(m_randomEngine);
}

bool Random::randBool()
{
	return (randFloat() <= 0.5) ? true : false;
}

std::wstring DINOGUI::toWideString(const std::string& string)
{
	return std::wstring(string.begin(), string.end());
}

void DINOGUI::throwIfFailed(HRESULT result, const std::string& message)
{
	if (FAILED(result))
	{
		std::cerr << "ERROR: " << message << std::endl;
		throw std::runtime_error(message);
	}
}

void DINOGUI::throwIfFailed(bool result, const std::string& message)
{
	if (!result)
	{
		std::cerr << "ERROR: " << message << std::endl;
		throw std::runtime_error(message);
	}
}
