#pragma once

#include "BaseWindow.h"

#include <d2d1.h>

namespace DINOGUI
{

class Widget
{
public:
	virtual void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) = 0;
	virtual void place(int row, int col, int rowSpan, int colSpan) = 0;
	virtual void hover(int x, int y) = 0;
};

}
