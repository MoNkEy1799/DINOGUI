#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Checkbox::Checkbox(Base* base, const std::string& text)
    : m_check(false)
{
    m_base = base;
    m_base->addWidget(this);
    m_type = WidgetType::CHECKBOX;
    m_text = text;
    m_drawBackground = 1;
    m_drawBorder = 1;
    m_size = { 80.0f, 20.0f };
}

void Checkbox::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush, ID2D1StrokeStyle* strokeStyle)
{
    D2D1_COLOR_F background = toD2DColorF(m_theme.bg);
    D2D1_COLOR_F border = toD2DColorF(m_theme.brd);
    D2D1_COLOR_F text = toD2DColorF(m_theme.txt);
    D2D1_RECT_F textRect = currentTextRect();
    D2D1_RECT_F boxRect = currentBoxRect();

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
        renderTarget->FillRectangle(boxRect, brush);
    }
    if (m_drawBorder)
    {
        brush->SetColor(border);
        renderTarget->DrawRectangle(boxRect, brush);
    }

    if (!m_fontFormat)
    {
        if (!createFontFormat())
        {
            throw std::runtime_error("Could not create Font Format");
            return;
        }
    }

    if (m_check)
    {

    }

    brush->SetColor(text);
    renderTarget->DrawText(toWideString(m_text).c_str(), m_text.size(), m_fontFormat, textRect, brush);
}

void Checkbox::place(int x, int y)
{
    show();
    m_point = D2D1::Point2F(DPIConverter::PixelsToDips(x), DPIConverter::PixelsToDips(y));
}

void Checkbox::clicked()
{
    m_check = !m_check;
}

D2D1_RECT_F Checkbox::currentTextRect()
{
    D2D1_RECT_F current = currentRect();
    return { current.left + 10.0f,
             current.top,
             current.right,
             current.bottom };
}

D2D1_RECT_F Checkbox::currentBoxRect()
{
    D2D1_RECT_F current = currentRect();
    float midHeight = current.top + (current.bottom - current.top) / 2.0f;
    return { current.left,
             midHeight - 4.0f,
             current.left + 10.f,
             midHeight + 6.0f };
}

