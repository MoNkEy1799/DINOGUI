#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>

using namespace DINOGUI;

Widget::Widget(Core* core)
    : m_core(core), m_theme(nullptr), m_state(WidgetState::NORMAL), m_type(WidgetType::NONE), m_resizeState(),
      m_point({ 0.0f, 0.0f }), m_size({ 60.0f, 20.0f }), m_minSize({ 0.0f, 0.0f }), m_maxSize({ 1e6f, 1e6f }),
      m_drawBackground(false), m_drawBorder(false), m_hoverable(false), m_clickable(false), m_holdable(false),
      m_selectable(false), m_checkable(false), m_checked(false), m_selected(false)
{
    m_core->addWidget(this);
    m_theme = new ColorTheme();
    m_resizeState.size = &m_size;
}

Widget::~Widget()
{
    m_core->removeWidget(this);
    m_core->removeDisplayWidget(this);
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
	bool inX = (x >= m_point.x && x <= m_point.x + m_size.width);
	bool inY = (y >= m_point.y && y <= m_point.y + m_size.height);
	return ( inX && inY );
}

void Widget::show()
{
    m_core->removeDisplayWidget(this);
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
        m_core->setHoverWidget(this);
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
    m_core->setHoverWidget(nullptr);
    m_core->setClickWidget(nullptr);
    m_core->redrawScreen();
}

void Widget::clickEvent(float mouseX, float mouseY)
{
    if (m_clickable || m_checkable)
    {
        m_state = WidgetState::CLICKED;
        m_core->setClickWidget(this);
        m_core->redrawScreen();
    }
    else if (m_selectable)
    {
        m_state = WidgetState::SELECTED_HOVER;
        m_selected = true;
        m_core->setSelectWidget(this);
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
        m_core->setHoverWidget(this);
        m_core->redrawScreen();
    }
    if (m_checkable)
    {
        m_checked = !m_checked;
        m_state = m_checked ? WidgetState::CHECKED : WidgetState::NORMAL;
    }
    clicked(mouseX, mouseY);
    m_core->setClickWidget(nullptr);
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
    m_core->setSelectWidget(nullptr);
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
