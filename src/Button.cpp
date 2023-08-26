#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Button::Button(Core* core, const std::string& text, std::function<void()> function)
    : Widget(core), m_clickFunction(function)
{
    m_type = WidgetType::BUTTON;
    m_text = text;
    m_drawBackground = true;
    m_drawBorder = true;

    std::cout << m_theme.bg.b << std::endl;
}

void Button::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    drawBasicShape(renderTarget, brush);
    D2D1_COLOR_F colText = Color::d2d1(m_theme.txt);
    D2D1_RECT_F rectangle = DPIHandler::adjusted(currentRect());

    if (!m_fontFormat)
    {
        throwIfFailed(createFontFormat(), "Failed to create text format");
    }

    brush->SetColor(colText);
    renderTarget->DrawText(toWideString(m_text).c_str(), (uint32_t)m_text.size(), m_fontFormat, rectangle, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void Button::place(int x, int y)
{
    basicPlace(x, y);
}

void Button::clicked(float mouseX, float mouseY)
{
    if (m_clickFunction)
    {
        m_clickFunction();
    }
}

void Button::connect(std::function<void()> function)
{
    m_clickFunction = function;
}
