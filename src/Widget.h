#pragma once

#include <d2d1.h>
#include <string>

namespace DINOGUI
{

class Widget
{
public:
	virtual void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) = 0;
	virtual void place(int row, int col, int rowSpan, int colSpan) = 0;

	static std::wstring toWideString(const std::string& string) { return std::wstring(string.begin(), string.end()); };
};

}
