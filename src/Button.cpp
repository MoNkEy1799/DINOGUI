#include "Button.h"

#include <d2d1.h>

DINOGUI::Button::Button(DINOGUI::Base& base)
	: Widget(base), m_color({ 1.0f, 0.0f, 0.0f })
{
	m_position = { 50, 50 };
	m_size = { 20, 20 };
}

void DINOGUI::Button::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
	renderTarget->FillRectangle(D2D1::RectF(50.0f, 50.0f, 70.0f, 70.0f), brush);
}
