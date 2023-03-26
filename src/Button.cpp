#include "Button.h"
#include "BaseWindow.h"
#include "Utils.h"

#include <d2d1.h>
#include <string>
#include <dwrite.h>

DINOGUI::Button::Button(DINOGUI::Base* base, std::string text, DINOGUI::Font font)
	: m_base(base), m_color(), m_rect(), m_fontFormat(nullptr), m_text(), m_font(font)
{
	m_text = toWideString(text);
	m_color = D2D1::ColorF(1.0f, 1.0f, 0.0f);
	m_rect = D2D1::RectF(60.0f, 60.0f, 100.0f, 100.0f);
}

DINOGUI::Button::~Button()
{
    safeReleaseInterface(&m_fontFormat);
}

void DINOGUI::Button::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
	brush->SetColor(m_color);
	renderTarget->FillRectangle(m_rect, brush);
    brush->SetColor(D2D1::ColorF(1.0f, 0.0f, 1.0f));

    if (!m_fontFormat)
    {
        createFontFormat();
    }

    renderTarget->DrawText(m_text.c_str(), m_text.size(), m_fontFormat, m_rect, brush);
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

void DINOGUI::Button::createFontFormat()
{
    HRESULT hResult = m_base->getWriteFactory()->CreateTextFormat(
        toWideString(m_font.family).c_str(), NULL, (DWRITE_FONT_WEIGHT)m_font.weight,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, m_font.size, L"en-us", &m_fontFormat);

    if (SUCCEEDED(hResult))
    {
        hResult = m_fontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    }

    if (SUCCEEDED(hResult))
    {
        hResult = m_fontFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
}
