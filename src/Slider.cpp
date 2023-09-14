#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

using namespace DINOGUI;

Slider::Slider(Core* core)
    : Widget(core), m_ticks(100), m_currentTick(0), m_vertical(false)
{
    m_type = WidgetType::SLIDER;
    m_drawBorder = true;
    m_size = { 200.0f, 20.0f };
}

void Slider::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_RECT_F rect = currentRect();
    rect = { rect.left, rect.top + 8.0f, rect.right, rect.bottom - 8.0f };
    brush->SetColor(Color::d2d1(m_theme->border[(int)m_state]));
    renderTarget->DrawRectangle(DPIHandler::adjusted(rect), brush);
    float x = m_vertical ? 10.0f : 5.0f;
    float y = m_vertical ? 5.0f : 10.0f;
    float midX = m_vertical ? rect.left + ((rect.right - rect.left) / 2.0f) : rect.left;
    float midY = m_vertical ? rect.top : rect.top + ((rect.bottom - rect.top) / 2.0f);
    if (m_vertical)
    {
        midY += m_currentTick * m_size.height / m_ticks;
    }
    else
    {
        midX += m_currentTick * m_size.width / m_ticks;
    }
    D2D1_RECT_F slider = { midX - x, midY - y, midX + x, midY + y };
    brush->SetColor(Color::d2d1(m_theme->border[(int)m_state]));
    renderTarget->FillRectangle(DPIHandler::adjusted(slider), brush);
}

void Slider::place(int x, int y)
{
    basicPlace(x, y);
}

void Slider::clicked(float mouseX, float mouseY)
{
}

void Slider::setMaxTicks(int ticks)
{
    int maxTicks = m_vertical ? m_size.height : m_size.width;
    m_ticks = std::min(ticks, maxTicks);
}

int Slider::getCurrentTick()
{
    return m_currentTick;
}

void Slider::moveSlider(float x, float y)
{

}

void Slider::setVertical()
{
    m_vertical = true;
    m_size = { m_size.height, m_size.width };
}
