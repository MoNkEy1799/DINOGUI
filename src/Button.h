#pragma once

#include "Widget.h"
#include "Utils.h"

#include <d2d1.h>

namespace DINOGUI
{

class Button : public Widget
{
public:
	Button(DINOGUI::Base* base);

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int row, int col, int rowSpan, int colSpan) override;
	void hover(int x, int y) override;

private:
	D2D1_RECT_F m_rect;
	D2D1_COLOR_F m_color;
	DINOGUI::Base* m_base;

	bool insideRect(int x, int y);
};

}
