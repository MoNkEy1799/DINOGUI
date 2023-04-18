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
	return (std::floor(f * m_scale) + 0.5f) / m_scale;
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

void DINOGUI::Timer::restart()
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

D2D1_COLOR_F DINOGUI::toD2DColorF(const Color& color)
{
	return D2D1::ColorF(color.r, color.g, color.b, color.a);
}

std::wstring DINOGUI::toWideString(const std::string& string)
{
	return std::wstring(string.begin(), string.end());
}
