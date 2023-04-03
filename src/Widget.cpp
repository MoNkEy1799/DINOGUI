#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>

using namespace DINOGUI;

Widget::Widget()
    : m_fontFormat(nullptr), m_base(nullptr), m_text(""),
      m_theme(DINOGUI_THEME_LIGHT), m_font(DINOGUI_FONT_DEFAULT),
      m_point({ 0.0f, 0.0f }), m_size({ 60.0f, 20.0f }),
      m_state(WidgetState::NORMAL), m_type(WidgetType::NONE),
      m_drawBackground(false), m_drawBorder(false), m_hover(false)
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

void Widget::setSize(int width, int height)
{
    m_size = { (float)width, (float)height };
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

void Widget::show()
{
    m_base->addDisplayWidget(this);
    m_base->redrawScreen();
}

void Widget::hide()
{
    m_base->removeDisplayWidget(this);
    m_base->redrawScreen();
}

void Widget::drawBorder(bool draw)
{
    m_drawBorder = draw;
}

void Widget::drawBackground(bool draw)
{
    m_drawBackground = draw;
}

void Widget::enterEvent()
{
    if (hoverableWidget(m_type))
    {
        m_state = WidgetState::HOVER;
        m_base->redrawScreen();
    }
}

void Widget::leaveEvent()
{
    if (hoverableWidget(m_type))
    {
        m_state = WidgetState::NORMAL;
        m_base->redrawScreen();
    }
}

void Widget::clickEvent()
{
    if (clickableWidget(m_type))
    {
        m_state = WidgetState::CLICKED;
        m_base->redrawScreen();
    }
}

void Widget::releaseEvent()
{
    clicked();
    if (hoverableWidget(m_type))
    {
        m_state = WidgetState::HOVER;
        m_base->redrawScreen();
    }
}

void Widget::selectEvent()
{
}

void Widget::unselectEvent()
{
}

D2D1_RECT_F Widget::currentRect()
{
    return { m_point.x, m_point.y, m_point.x + m_size.width, m_point.y + m_size.height };
}

D2D1_RECT_F Widget::drawingAdjusted(D2D1_RECT_F rect)
{
    return { rect.left + 0.3f, rect.top + 0.3f, rect.right + 0.3f, rect.bottom + 0.3f };
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

bool Widget::hoverableWidget(const WidgetType& type)
{
    switch (type)
    {
    case WidgetType::LABEL:
    case WidgetType::IMAGE:
    case WidgetType::NONE:
        return false;
    }
    return true;
}

bool Widget::clickableWidget(const WidgetType& type)
{
    switch (type)
    {
    case WidgetType::LABEL:
    case WidgetType::IMAGE:
    case WidgetType::NONE:
        return false;
    }
    return true;
}
bool Widget::selectableWidget(const WidgetType& type)
{
    switch (type)
    {
    case WidgetType::LABEL:
    case WidgetType::IMAGE:
    case WidgetType::BUTTON:
    case WidgetType::NONE:
        return false;
    }
    return true;
}