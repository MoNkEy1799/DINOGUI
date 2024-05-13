#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

using namespace DINOGUI;

Slider::Slider(Core* core, bool vertical)
    : Widget(core), m_ticks(0), m_currentTick(0), m_vertical(vertical)
{
    m_type = WidgetType::SLIDER;
    ColorTheme::createDefault(m_theme, m_type);
    m_drawBorder = true;
    m_hoverable = true;
    m_clickable = true;
    m_holdable = true;
    m_size = { 160.0f, 20.0f };
    m_maxSize = { 1e6f, 1e6f };
    m_ticks = (int)m_size.width;
    if (vertical)
    {
        m_size = { m_size.height, m_size.width };
    }
}

void Slider::draw()
{
    D2D1_RECT_F rect = currentRect();
    ID2D1HwndRenderTarget* renderTarget = getRenderTarget(m_core);
    ID2D1SolidColorBrush* brush = getColorBrush(m_core);
    if (m_vertical)
    {
        rect.left += 8.0f;
        rect.right -= 8.0f;
        rect.top += 5.0f;
        rect.bottom -= 5.0f;
    }
    else
    {
        rect.top += 8.0f;
        rect.bottom -= 8.0f;
        rect.left += 5.0f;
        rect.right -= 5.0f;
    }
    brush->SetColor(Color::d2d1(m_theme->border2[(int)m_state]));
    renderTarget->DrawRectangle(DPIHandler::adjusted(rect), brush, m_theme->width);
    float x = m_vertical ? 10.0f : 5.0f;
    float y = m_vertical ? 5.0f : 10.0f;
    float midX = m_vertical ? rect.left + ((rect.right - rect.left) / 2.0f) : rect.left;
    float midY = m_vertical ? rect.top : rect.top + ((rect.bottom - rect.top) / 2.0f);
    if (m_vertical)
    {
        midY += m_currentTick * (m_size.height - 10.0f) / m_ticks;
    }
    else
    {
        midX += m_currentTick * (m_size.width -10.0f) / m_ticks;
    }
    D2D1_RECT_F slider = { midX - x, midY - y, midX + x, midY + y };
    brush->SetColor(Color::d2d1(m_theme->background[(int)m_state]));
    renderTarget->FillRectangle(DPIHandler::adjusted(slider), brush);
}

void Slider::place(int x, int y)
{
    basicPlace(x, y);
}

void Slider::clicked(float mouseX, float mouseY, bool hold)
{
    float pos = m_vertical ? mouseY - m_point.y : mouseX - m_point.x;
    float param = m_vertical ? m_size.height : m_size.width;
    pos = limitRange(pos, 0.0f, param);
    m_currentTick = (int)std::round(pos / (param / m_ticks));
}

void Slider::setMaxTicks(int ticks)
{
    int maxTicks = m_vertical ? (int)m_size.height - 10 : (int)m_size.width - 10;
    m_ticks = limitRange(ticks, 2, maxTicks);
}

int Slider::getCurrentTick()
{
    return m_currentTick;
}

float Slider::getCurrentPercentage()
{
    return (float)m_currentTick / m_ticks;
}

void Slider::setVertical(bool vertical)
{
    if (m_vertical == vertical)
    {
        return;
    }
    m_vertical = vertical;
    m_size = { m_size.height, m_size.width };
}
