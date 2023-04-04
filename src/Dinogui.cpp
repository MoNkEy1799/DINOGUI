#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <cmath>

using namespace DINOGUI;

float DPIConverter::scale = 1.0f;

D2D1_COLOR_F DINOGUI::toD2DColorF(const Color& color)
{
	return D2D1::ColorF(color.r, color.g, color.b, color.a);
}

void DPIConverter::Initialize(HWND windowHandle)
{
    uint32_t dpi = GetDpiForWindow(windowHandle);
    scale = dpi / 96.0f;

	std::cout << "DPI Scale : " << scale << "\n" << std::endl;
}

float DPIConverter::PixelsToDips(float f)
{
	return f / scale;
}

float DPIConverter::DpiAdjusted(float f)
{
	if (!std::fmod(f * scale, 1.0f))
	{
		return f + (0.5f / scale);
	}
	return f;
}

std::wstring DINOGUI::toWideString(const std::string& string)
{
	return std::wstring(string.begin(), string.end());
}

