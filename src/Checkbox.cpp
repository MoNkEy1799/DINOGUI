#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Checkbox::Checkbox(Base* base, const std::string& text)
    : m_check(false), m_boxPoint(m_point), m_textPoint(m_point),
      m_boxSize({ 12.0f, 12.0f }), m_textSize(m_size)
{
    m_base = base;
    m_base->addWidget(this);
    m_type = WidgetType::CHECKBOX;
    m_text = text;
    m_drawBackground = 1;
    m_drawBorder = 1;
    m_size = { 80.0f, 20.0f };
}

void DINOGUI::Checkbox::setSize(int width, int height)
{
    m_size = { (float)width, (float)height };
    calculateBoxAndTextLayout();
}

void Checkbox::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush, ID2D1StrokeStyle* strokeStyle)
{
    D2D1_COLOR_F background = toD2DColorF(m_theme.bg);
    D2D1_COLOR_F border = toD2DColorF(m_theme.brd);
    D2D1_COLOR_F text = toD2DColorF(m_theme.txt);
    D2D1_RECT_F textRect = drawingAdjusted(currentTextRect());
    D2D1_RECT_F boxRect = drawingAdjusted(currentBoxRect());

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
        }
        if (!SUCCEEDED(m_fontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)))
        {
            throw std::runtime_error("Could not align Font Format");
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
    calculateBoxAndTextLayout();
}

void Checkbox::clicked()
{
    m_check = !m_check;
}

void DINOGUI::Checkbox::calculateBoxAndTextLayout()
{
    D2D1_RECT_F current = currentRect();
    float pad = m_boxSize.width / 4.0f;
    float midHeight = m_size.height / 2.0f;
    m_boxPoint = { m_point.x + pad, m_point.y + midHeight - m_boxSize.height / 2.0f };
    m_textPoint = { m_point.x + 2 * pad + m_boxSize.width, m_point.y };
    m_textSize = { m_size.width - 2 * pad + m_boxSize.width, m_size.height };
}

D2D1_RECT_F Checkbox::currentTextRect()
{
    return { m_textPoint.x, m_textPoint.y, m_textPoint.x + m_textSize.width, m_textPoint.y + m_textSize.height };
}

D2D1_RECT_F Checkbox::currentBoxRect()
{
    return { m_boxPoint.x, m_boxPoint.y, m_boxPoint.x + m_boxSize.width, m_boxPoint.y + m_boxSize.height };
}

