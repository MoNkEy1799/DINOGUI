#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>

using namespace DINOGUI;

Widget::Widget()
    : m_fontFormat(nullptr), m_base(nullptr),
      m_theme(DINOGUI_THEME_LIGHT), m_font(DINOGUI_FONT_DEFAULT),
      m_point({ 0.0f, 0.0f }), m_size({ 60.0f, 20.0f }),
      m_state(WidgetState::NORMAL), m_type(WidgetType::NONE)
{
}

Widget::~Widget()
{
    m_base->removeWidget(this);
    safeReleaseInterface(&m_fontFormat);
}

void Widget::setTheme(const ColorTheme& theme)
{
	m_theme = theme;
    m_base->redrawScreen();
}

void Widget::setFont(const Font& font)
{
    m_font = font;
    safeReleaseInterface(&m_fontFormat);
    m_base->redrawScreen();
}

void Widget::setWidgetState(const WidgetState& state, bool redraw)
{
	m_state = state;
    if (redraw)
    {
        m_base->redrawScreen();
    }
}

WidgetType Widget::getWidgetType()
{
    return m_type;
}

bool Widget::contains(int x, int y)
{
	bool inX = (x > m_point.x && x < m_point.x + m_size.width);
	bool inY = (y > m_point.y && y < m_point.y + m_size.height);
	return ( inX && inY );
}

bool Widget::createFontFormat()
{
    HRESULT hResult = m_base->getWriteFactory()->CreateTextFormat(
        toWideString(m_font.family).c_str(), NULL, (DWRITE_FONT_WEIGHT)m_font.weight,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, m_font.size, L"en-us", &m_fontFormat);

    if (FAILED(hResult))
    {
        return false;
    }

    if (!SUCCEEDED(m_fontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER)))
    {
        return false;
    }

    if (!SUCCEEDED(m_fontFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER)))
    {
        return false;
    }

    return true;
}

void DINOGUI::Widget::show()
{
    m_base->addDisplayWidget(this);
    m_base->redrawScreen();
}

void DINOGUI::Widget::hide()
{
    m_base->removeDisplayWidget(this);
    m_base->redrawScreen();
}

D2D1_RECT_F Widget::currentRect()
{
    return D2D1::Rect(m_point.x, m_point.y, m_point.x + m_size.width, m_point.y + m_size.height);
}