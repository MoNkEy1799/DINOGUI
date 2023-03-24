#pragma once

#include "BaseWindow.h"

#include <d2d1.h>

namespace DINOGUI
{

class Widget
{
public:
	virtual void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) = 0;
	virtual void place(Widget& widget, int row, int col) = 0;
};

}
