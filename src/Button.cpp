#include "Button.h"

#include <d2d1.h>

DINOGUI::Button::Button(DINOGUI::Base* base)
	: m_base(base)
{
	m_color = D2D1::ColorF(1.0f, 1.0f, 0.0f);
	m_rect = D2D1::RectF(60.0f, 60.0f, 100.0f, 100.0f);
}

void DINOGUI::Button::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
	brush->SetColor(m_color);
	renderTarget->FillRectangle(m_rect, brush);
}

void DINOGUI::Button::place(int row, int col, int rowSpan, int colSpan)
{
	m_base->addChild(this);
}

void DINOGUI::Button::hover(int x, int y)
{
	if (insideRect(x, y))
	{
		m_color = D2D1::ColorF(1.0f, 0.0f, 0.0f);
		InvalidateRect(m_base->getWindowHandle(), nullptr, false);
	}
}

bool DINOGUI::Button::insideRect(int x, int y)
{
	bool containsX = (x > m_rect.left && x < m_rect.right);
	bool containsY = (y > m_rect.top && y < m_rect.bottom);
	return (containsX && containsY);
}
