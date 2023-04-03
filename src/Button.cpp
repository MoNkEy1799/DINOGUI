#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Button::Button(Base* base, const std::string& text, std::function<void()> function)
    : m_clickFunction(function)
{
    m_base = base;
    m_base->addWidget(this);
    m_type = WidgetType::BUTTON;
    m_text = text;
    m_drawBackground = true;
    m_drawBorder = true;
}

void Button::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush, ID2D1StrokeStyle* strokeStyle)
{
    D2D1_COLOR_F background;
    D2D1_COLOR_F border;
    D2D1_COLOR_F text;
    D2D1_RECT_F rectangle = drawingAdjusted(currentRect());
    std::cout << rectangle.left << " , " << rectangle.top << std::endl;
    std::cout << rectangle.right << " , " << rectangle.bottom << std::endl;

    switch (m_state)
    {
    case WidgetState::SELECTED:
    case WidgetState::NORMAL:
        background = toD2DColorF(m_theme.bg);
        border = toD2DColorF(m_theme.brd);
        text = toD2DColorF(m_theme.txt);
        break;

    case WidgetState::HOVER:
        background = toD2DColorF(m_theme.bg_h);
        border = toD2DColorF(m_theme.brd_h);
        text = toD2DColorF(m_theme.txt_h);
        break;

    case WidgetState::CLICKED:
        background = toD2DColorF(m_theme.bg_c);
        border = toD2DColorF(m_theme.brd_c);
        text = toD2DColorF(m_theme.txt_c);
        break;
    }

    if (m_drawBackground)
    {
        brush->SetColor(background);
        renderTarget->FillRectangle(rectangle, brush);
    }
    if (m_drawBorder)
    {
        brush->SetColor(border);
        renderTarget->DrawRectangle(rectangle, brush);
    }

    if (!m_fontFormat)
    {
        if (!createFontFormat())
        {
            throw std::runtime_error("Could not create Font Format");
            return;
        }
    }

    brush->SetColor(text);
    renderTarget->DrawText(toWideString(m_text).c_str(), m_text.size(), m_fontFormat, rectangle, brush);
}

void Button::place(int x, int y)
{
    show();
    m_point = D2D1::Point2F(DPIConverter::PixelsToDips(x), DPIConverter::PixelsToDips(y));
}

void Button::clicked()
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
