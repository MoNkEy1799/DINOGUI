#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>

using namespace DINOGUI;

void Widget::DEBUG_PRINT_COORDS(D2D1_RECT_F rect, const std::string& str)
{
    std::cout << "### Adj. Coords for " << str << " ###" << std::endl;
    std::cout << "top-left: " << rect.left << ", " << rect.top << std::endl;
    std::cout << "bottom-right: " << rect.right << ", " << rect.bottom << std::endl;
    std::cout << std::endl;
    std::cout << "### Act. Coords for " << str << " ###" << std::endl;
    std::cout << "top-left: " << rect.left * 1.0f / DPIHandler::PixelsToDips(1.0f) << ", " << rect.top * 1.0f / DPIHandler::PixelsToDips(1.0f) << std::endl;
    std::cout << "bottom-right: " << rect.right * 1.0f / DPIHandler::PixelsToDips(1.0f) << ", " << rect.bottom * 1.0f / DPIHandler::PixelsToDips(1.0f) << std::endl;
}

Widget::Widget(Core* core)
    : m_fontFormat(nullptr), m_core(core), m_text(""),
      m_theme(DINOGUI_THEME_LIGHT), m_font(DINOGUI_FONT_DEFAULT),
      m_point({ 0.0f, 0.0f }), m_size({ 60.0f, 20.0f }),
      m_state(WidgetState::NORMAL), m_type(WidgetType::NONE),
      m_drawBackground(false), m_drawBorder(false), m_hover(false)
{
    m_core->addWidget(this);
}

Widget::~Widget()
{
    m_core->removeWidget(this);
    m_core->removeDisplayWidget(this);
    safeReleaseInterface(&m_fontFormat);
}

void Widget::setTheme(const ColorTheme& theme)
{
	m_theme = theme;
}

void Widget::setText(const std::string& text)
{
    m_text = text;
}

void Widget::setFont(const Font& font)
{
    m_font = font;
    safeReleaseInterface(&m_fontFormat);
}

void Widget::setSize(int width, int height)
{
    m_size = { (float)width, (float)height };
}

WidgetType Widget::getWidgetType() const
{
    return m_type;
}

bool Widget::contains(float x, float y) const
{
	bool inX = (x > m_point.x && x < m_point.x + m_size.width);
	bool inY = (y > m_point.y && y < m_point.y + m_size.height);
	return ( inX && inY );
}

void Widget::show()
{
    m_core->addDisplayWidget(this);
    m_core->redrawScreen();
}

void Widget::hide()
{
    m_core->removeDisplayWidget(this);
    m_core->redrawScreen();
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
        m_core->setHoverWidget(this);
        m_core->redrawScreen();
    }
}

void Widget::leaveEvent()
{
    m_state = WidgetState::NORMAL;
    m_core->setHoverWidget(nullptr);
    m_core->setClickWidget(nullptr);
    m_core->redrawScreen();
}

void Widget::clickEvent()
{
    if (clickableWidget(m_type))
    {
        m_state = WidgetState::CLICKED;
        m_core->setClickWidget(this);
        m_core->redrawScreen();
    }
    else if (selectableWidget(m_type))
    {
        clicked();
        m_core->setSelectedWidget(this);
        m_core->redrawScreen();
    }
}

void Widget::releaseEvent()
{
    if (hoverableWidget(m_type))
    {
        m_state = WidgetState::HOVER;
        m_core->setHoverWidget(this);
        m_core->redrawScreen();
    }
    clicked();
    m_core->setClickWidget(nullptr);
}

void DINOGUI::Widget::unselectEvent()
{
    clicked();
    m_state = WidgetState::NORMAL;
    m_core->setSelectedWidget(nullptr);
    m_core->redrawScreen();
}

D2D1_RECT_F Widget::currentRect() const
{
    return { m_point.x, m_point.y, m_point.x + m_size.width, m_point.y + m_size.height };
}

bool Widget::createFontFormat()
{
    HRESULT hResult = m_core->getWriteFactory()->CreateTextFormat(
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
    case WidgetType::NONE:
    case WidgetType::LABEL:
    case WidgetType::IMAGE:
        return false;
    }
    return true;
}

bool Widget::clickableWidget(const WidgetType& type)
{
    switch (type)
    {
    case WidgetType::NONE:
    case WidgetType::LABEL:
    case WidgetType::TEXTEDIT:
    case WidgetType::IMAGE:
        return false;
    }
    return true;
}
bool Widget::selectableWidget(const WidgetType& type)
{
    switch (type)
    {
    case WidgetType::NONE:
    case WidgetType::BUTTON:
    case WidgetType::LABEL:
    case WidgetType::CHECKBOX:
    case WidgetType::IMAGE:
        return false;
    }
    return true;
}