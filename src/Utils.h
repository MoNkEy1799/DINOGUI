#pragma once

#include <d2d1.h>
#include <array>

namespace DINOGUI
{


struct DPIConverter
{
    static void Initialize(HWND windowHandle)
    {
        uint32_t dpi = GetDpiForWindow(windowHandle);
        scaleX = dpi / 96.0f;
        scaleY = dpi / 96.0f;
    }

    template <typename T>
    static float PixelsToDipsX(T x)
    {
        return static_cast<float>(x) / scaleX;
    }

    template <typename T>
    static float PixelsToDipsY(T y)
    {
        return static_cast<float>(y) / scaleY;
    }

private:
    static float scaleX;
    static float scaleY;
};

struct Rect
{
	int x, y, width, height;
};

struct Color
{
	float r, g, b;
};

}
