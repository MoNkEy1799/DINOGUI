#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Checkbox::Checkbox(Base* base, const std::string& text)
    : m_check(false), m_box({0.0f, 0.0f, 0.0f, 0.0f})
{
    m_base = base;
    m_base->addWidget(this);
    m_type = WidgetType::LABEL;
    m_text = text;
    m_drawBackground = false;
    m_drawBorder = false;
}

void Checkbox::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_COLOR_F background = toD2DColorF(m_theme.bg);
    D2D1_COLOR_F border = toD2DColorF(m_theme.brd);
    D2D1_COLOR_F text = toD2DColorF(m_theme.txt);
    D2D1_RECT_F rectangle = currentRect();

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

void Checkbox::grid(int row, int col, int rowSpan, int colSpan)
{
    show();
}

void Checkbox::place(int x, int y)
{
    show();
    m_point = D2D1::Point2F(DPIConverter::PixelsToDips(x), DPIConverter::PixelsToDips(y));
}

void Checkbox::check()
{
    m_check = !m_check;
}
