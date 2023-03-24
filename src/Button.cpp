#include "Button.h"

#include <d2d1.h>

DINOGUI::Button::Button(DINOGUI::Base& base)
	: m_base(base)
{
}

void DINOGUI::Button::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
	renderTarget->FillRectangle(D2D1::RectF(50.0f, 50.0f, 70.0f, 70.0f), brush);
}

void DINOGUI::Button::place(Widget& widget, int row, int col)
{
}
