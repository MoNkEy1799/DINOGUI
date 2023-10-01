#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>

using namespace DINOGUI;

Widget::Widget(Core* core)
    : LayoutObject(LayoutObjectType::WIDGET), m_core(core), m_theme(nullptr), m_state(WidgetState::NORMAL), m_type(WidgetType::NONE),
      m_point({ 0.0f, 0.0f }), m_size({ 60.0f, 20.0f }), m_minSize({ 10.0f, 10.0f }), m_maxSize({ 1e6f, 20.0f }),
      m_drawBackground(false), m_drawBorder(false), m_hoverable(false), m_clickable(false), m_holdable(false),
      m_selectable(false), m_checkable(false), m_checked(false), m_selected(false), resizeState(m_size, m_minSize, m_maxSize)
{
    CoreInterface::addWidget(m_core, this);
    m_theme = new ColorTheme();
}

Widget::~Widget()
{
    CoreInterface::removeWidget(m_core, this);
    CoreInterface::removeDisplayWidget(m_core, this);
    delete m_theme;
}

void Widget::centerPlace(int x, int y)
{
    place(x - (int)m_size.width, y - (int)m_size.height);
}

void Widget::setTheme(ColorTheme* theme)
{
	m_theme = theme;
}

void Widget::resize(int width, int height)
{
    float w = limitRange((float)width, m_minSize.width, m_maxSize.width);
    float h = limitRange((float)height, m_minSize.height, m_maxSize.height);
    m_size = { w, h };
}

void Widget::setFixedSize(int width, int height)
{
    m_minSize = { limitRange((float)width, 0.0f, 1e6f), limitRange((float)height, 0.0f, 1e6f) };
    m_maxSize = { limitRange((float)width, 0.0f, 1e6f), limitRange((float)height, 0.0f, 1e6f) };
}

void Widget::setMinimumSize(int width, int height)
{
    m_minSize = { limitRange((float)width, 0.0f, m_maxSize.width), limitRange((float)height, 0.0f, m_maxSize.height) };
}

void Widget::setMaximumSize(int width, int height)
{
    m_maxSize = { limitRange((float)width, m_minSize.width, 1e6f), limitRange((float)height, m_minSize.height, 1e6f) };
}

WidgetType Widget::getWidgetType() const
{
    return m_type;
}

bool Widget::contains(float x, float y) const
{
	bool inX = (x >= m_point.x && x <= m_point.x + m_size.width);
	bool inY = (y >= m_point.y && y <= m_point.y + m_size.height);
	return ( inX && inY );
}

void Widget::show()
{
    CoreInterface::addDisplayWidget(m_core, this);
    m_core->redrawScreen();
}

void Widget::hide()
{
    CoreInterface::removeDisplayWidget(m_core, this);
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

void Widget::receiveEvent(Event* event)
{
    switch (event->type)
    {
    case EventType::ENTER_EVENT:
        enterEvent();
        break;

    case EventType::LEAVE_EVENT:
        leaveEvent();
        break;
    
    case EventType::CLICK_EVENT:
        clickEvent(event->mouseX, event->mouseY);
        break;

    case EventType::REALEASE_EVENT:
        releaseEvent(event->mouseX, event->mouseY);
        break;

    case EventType::UNSELECT_EVENT:
        unselectEvent();
        break;

    case EventType::HOLD_EVENT:
        holdEvent(event->mouseX, event->mouseY);
        break;
    }

    delete event;
}

void Widget::enterEvent()
{
    if (m_hoverable)
    {
        m_state = WidgetState::HOVER;
        if (m_checkable && m_checked)
        {
            WidgetState::CHECKED_HOVER;
        }
        else if (m_selectable && m_selected)
        {
            m_state = WidgetState::SELECTED_HOVER;
        }
        CoreInterface::setHoverWidget(m_core, this);
        m_core->redrawScreen();
    }
}

void Widget::leaveEvent()
{
    m_state = WidgetState::NORMAL;
    if (m_checkable && m_checked)
    {
        m_state = WidgetState::CHECKED;
    }
    else if (m_selectable && m_selected)
    {
        m_state = WidgetState::SELECTED;
    }
    CoreInterface::setHoverWidget(m_core, nullptr);
    CoreInterface::setClickWidget(m_core, nullptr);
    m_core->redrawScreen();
}

void Widget::clickEvent(float mouseX, float mouseY)
{
    if (m_clickable || m_checkable)
    {
        m_state = WidgetState::CLICKED;
        CoreInterface::setClickWidget(m_core, this);
        m_core->redrawScreen();
    }
    else if (m_selectable)
    {
        m_state = WidgetState::SELECTED_HOVER;
        m_selected = true;
        CoreInterface::setSelectWidget(m_core, this);
        clicked(mouseX, mouseY);
        m_core->redrawScreen();
    }
    if (m_holdable)
    {
        clicked(mouseX, mouseY);
    }
}

void Widget::holdEvent(float mouseX, float mouseY)
{
    if (m_holdable)
    {
        clicked(mouseX, mouseY);
        m_core->redrawScreen();
    }
}

void Widget::releaseEvent(float mouseX, float mouseY)
{
    m_state = WidgetState::NORMAL;
    if (m_hoverable)
    {
        m_state = WidgetState::HOVER;
        CoreInterface::setHoverWidget(m_core, this);
        m_core->redrawScreen();
    }
    if (m_checkable)
    {
        m_checked = !m_checked;
        m_state = m_checked ? WidgetState::CHECKED : WidgetState::NORMAL;
    }
    clicked(mouseX, mouseY);
    CoreInterface::setClickWidget(m_core, nullptr);
}

void Widget::unselectEvent()
{
    if (m_type == WidgetType::TEXTEDIT)
    {
        dynamic_cast<Textedit*>(this)->stopCursorTimer();
    }
    else if (m_type == WidgetType::COMBOBOX)
    {
        dynamic_cast<Combobox*>(this)->setDropdown(false);
    }
    m_state = WidgetState::NORMAL;
    m_selected = false;
    CoreInterface::setSelectWidget(m_core, nullptr);
    m_core->redrawScreen();
}

D2D1_RECT_F Widget::currentRect() const
{
    return { m_point.x, m_point.y, m_point.x + m_size.width, m_point.y + m_size.height };
}

void Widget::basicDrawBackgroundBorder(const D2D1_RECT_F& rect, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    if (m_drawBackground)
    {
        brush->SetColor(Color::d2d1(m_theme->background[(int)m_state]));
        renderTarget->FillRectangle(rect, brush);
    }
    if (m_drawBorder)
    {
        brush->SetColor(Color::d2d1(m_theme->border[(int)m_state]));
        renderTarget->DrawRectangle(rect, brush, m_theme->width);
    }
}

void Widget::basicPlace(int x, int y)
{
    m_point = D2D1::Point2F((float)x, (float)y);
    show();
}
