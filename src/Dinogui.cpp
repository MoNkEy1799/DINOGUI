#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <cmath>
#include <functional>

using namespace DINOGUI;

float DPIConverter::m_scale = 1.0f;

void DPIConverter::Initialize(HWND windowHandle)
{
    uint32_t dpi = GetDpiForWindow(windowHandle);
    m_scale = dpi / 96.0f;
	//std::cout << "DPI Scale : " << m_scale << "\n" << std::endl;
}

float DPIConverter::PixelsToDips(float f)
{
	return f / m_scale;
}

float DPIConverter::DpiAdjusted(float f)
{
	if (!std::fmod(f * m_scale, 1.0f))
	{
		return f + (0.5f / m_scale);
	}
	return f;
}

uint32_t Timer::m_totalTimers = 0;

Timer::Timer(HWND windowHandle, uint32_t timeout, std::function<void()> callback)
	: timeoutDelay(timeout), callback(callback), m_active(false), m_windowHandle(windowHandle)
{
	m_id = m_totalTimers;
	m_totalTimers++;
}

uint32_t Timer::getId() const
{
	return m_id;
}

void Timer::start()
{
	SetTimer(m_windowHandle, m_id, timeoutDelay, (TIMERPROC)&callback);
	m_active = true;
}

void Timer::stop()
{
	KillTimer(m_windowHandle, m_id);
	m_active = false;
}

bool Timer::isActive()
{
	return m_active;
}

D2D1_COLOR_F DINOGUI::toD2DColorF(const Color& color)
{
	return D2D1::ColorF(color.r, color.g, color.b, color.a);
}

std::wstring DINOGUI::toWideString(const std::string& string)
{
	return std::wstring(string.begin(), string.end());
}
