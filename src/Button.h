#pragma once

#include "Widget.h"
#include "Utils.h"

#include <d2d1.h>

namespace DINOGUI
{

class Button : public Widget
{
public:
	Button(DINOGUI::Base& base);

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(Widget& widget, int row, int col) override;

private:
	DINOGUI::Rect m_rect;
	DINOGUI::Color m_color;
	DINOGUI::Base m_base;
};

}
