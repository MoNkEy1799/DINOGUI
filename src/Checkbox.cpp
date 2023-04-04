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
      m_boxSize({ 12.0f, 12.0f }), m_textSize(m_size),
      m_checkmark(nullptr)
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
        brush->SetColor({ 1.0f, 1.0f, 1.0f });
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
        if (!m_checkmark && !createPathGeometry())
        {
            throw std::runtime_error("Could not create Path Geometry");
        }
        renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        brush->SetColor(border);
        renderTarget->FillGeometry(m_checkmark, brush);
        renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
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

bool Checkbox::createPathGeometry()
{
    ID2D1GeometrySink* sink;
    if (!SUCCEEDED(m_base->getFactory()->CreatePathGeometry(&m_checkmark)))
    {
        return false;
    }
    if (!SUCCEEDED(m_checkmark->Open(&sink)))
    {
        return false;
    }

    float x = m_boxPoint.x;
    float y = m_boxPoint.y;
    sink->BeginFigure(drawingAdjusted(D2D1::Point2F(10.5f + x, 3.5f + y)), D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(drawingAdjusted(D2D1::Point2F(11.5f + x, 4.5f + y)));
    sink->AddLine(drawingAdjusted(D2D1::Point2F(5.5f + x, 10.5f + y)));
    sink->AddLine(drawingAdjusted(D2D1::Point2F(1.5f + x, 6.5f + y)));
    sink->AddLine(drawingAdjusted(D2D1::Point2F(2.5f + x, 5.5f + y)));
    sink->AddLine(drawingAdjusted(D2D1::Point2F(5.5f + x, 8.5f + y)));
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);

    if (!SUCCEEDED(sink->Close()))
    {
        return false;
    }
    safeReleaseInterface(&sink);
    return true;
}

void Checkbox::calculateBoxAndTextLayout()
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

