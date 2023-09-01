#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <cmath>
#include <functional>
#include <string>

using namespace DINOGUI;

Text::Text(Core* core, const std::string& text)
	: font(DINOGUI_FONT_DEFAULT), hAlign(H_TextAlignment::CENTER), vAlign(V_TextAlignment::CENTER),
	color({ 0, 0, 0 }), m_fontFormat(nullptr), m_core(core)
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
	}

	brush->SetColor(Color::d2d1(m_color));
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
}

void Text::setHorizontalAlignment(H_TextAlignment hAlign)
{
	m_hAlign = hAlign;
	safeReleaseInterface(&m_fontFormat);
}

void Text::setVerticalAlignment(V_TextAlignment vAlign)
{
	m_vAlign = vAlign;
	safeReleaseInterface(&m_fontFormat);
}

bool Text::createFontFormat()
{
	HRESULT hResult = m_core->getWriteFactory()->CreateTextFormat(
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
	return { DpiAdjusted(point.x), DpiAdjusted(point.y) };
}

D2D1_RECT_F DPIHandler::adjusted(D2D1_RECT_F rect)
{
	return { DpiAdjusted(rect.left), DpiAdjusted(rect.top),
			 DpiAdjusted(rect.right, -0.5f), DpiAdjusted(rect.bottom, -0.5f) };
}

float DPIHandler::getScale()
{
	return m_scale;
}

float DPIHandler::DpiAdjusted(float f, float dir)
{
	return (std::floor(f * m_scale) + dir) / m_scale;
}

Timer::Timer(HWND windowHandle, uint32_t timeout, std::function<void()> callback)
	: timeoutDelay(timeout), callback(callback), m_active(false), m_windowHandle(windowHandle)
{
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

std::wstring DINOGUI::toWideString(const std::string& string)
{
	return std::wstring(string.begin(), string.end());
}

void DINOGUI::throwIfFailed(HRESULT result, const std::string& message)
{
	if (FAILED(result))
	{
		std::cout << message << std::endl;
		throw std::runtime_error(message);
	}
}

void DINOGUI::throwIfFailed(bool result, const std::string& message)
{
	if (!result)
	{
		std::cout << message << std::endl;
		throw std::runtime_error(message);
	}
}
