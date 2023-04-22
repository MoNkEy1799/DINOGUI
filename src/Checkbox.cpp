#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Checkbox::Checkbox(Core* core, const std::string& text)
    : Widget(core), m_check(false), m_boxPoint(m_point), m_textPoint(m_point),
      m_boxSize({ 12.0f, 12.0f }), m_textSize(m_size)
{
    m_type = WidgetType::CHECKBOX;
    m_text = text;
    m_drawBackground = false;
    m_drawBorder = false;
    m_size = { 80.0f, 20.0f };
}

void Checkbox::setSize(int width, int height)
{
    m_size = { (float)width, (float)height };
    calculateBoxAndTextLayout();
}

void Checkbox::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    drawBasicShape(renderTarget, brush);
    D2D1_COLOR_F background = toD2DColorF(m_theme.bg);
    D2D1_COLOR_F border = toD2DColorF(m_theme.brd);
    D2D1_COLOR_F text = toD2DColorF(m_theme.txt);
    D2D1_RECT_F textRect = DPIHandler::adjusted(currentTextRect());
    D2D1_RECT_F boxRect = DPIHandler::adjusted(currentBoxRect());

    switch (m_state)
    {
    case WidgetState::NORMAL:
        background = toD2DColorF(Color(1.0f, 1.0f, 1.0f));
        border = toD2DColorF(Color(0.2f, 0.2f, 0.2f));
        text = toD2DColorF(m_theme.txt);
        break;

    case WidgetState::HOVER:
        background = toD2DColorF(m_theme.bg_h);
        border = toD2DColorF(Color(0.0f, 0.4706f, 0.8431f));
        text = toD2DColorF(m_theme.txt_h);
        break;

    case WidgetState::CLICKED:
        background = toD2DColorF(m_theme.bg_c);
        border = toD2DColorF(m_theme.brd_c);
        text = toD2DColorF(m_theme.txt_c);
        break;
    }

    brush->SetColor(background);
    renderTarget->FillRectangle(boxRect, brush);
    brush->SetColor(border);
    renderTarget->DrawRectangle(boxRect, brush);

    if (!m_fontFormat)
    {
        throwIfFailed(createFontFormat(), "Failed to create text format");
        throwIfFailed(m_fontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING), "Failed to align text format");
    }

    if (m_check)
    {
        renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        brush->SetColor(border);
        std::array<D2D1_POINT_2F, 3> box = currentCheckbox();
        renderTarget->DrawLine(box[0], box[1], brush);
        renderTarget->DrawLine(box[1], box[2], brush);
        renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    }

    brush->SetColor(text);
    renderTarget->DrawText(toWideString(m_text).c_str(), (uint32_t)m_text.size(), m_fontFormat, textRect, brush);
}

void Checkbox::place(int x, int y)
{
    basicPlace(x, y);
    calculateBoxAndTextLayout();
}

void Checkbox::clicked(float mouseX, float mouseY)
{
    m_check = !m_check;
}

std::array<D2D1_POINT_2F, 3> Checkbox::currentCheckbox()
{
    D2D1_RECT_F box = currentBoxRect();
    return { { {box.left + 10.5f, box.top + 3.5f}, { box.left + 5.0f, box.top + 10.0f }, { box.left + 2.0f, box.top + 7.0f } } };
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

D2D1_RECT_F Checkbox::currentTextRect() const
{
    return { m_textPoint.x, m_textPoint.y, m_textPoint.x + m_textSize.width, m_textPoint.y + m_textSize.height };
}

D2D1_RECT_F Checkbox::currentBoxRect() const
{
    return { m_boxPoint.x, m_boxPoint.y, m_boxPoint.x + m_boxSize.width, m_boxPoint.y + m_boxSize.height };
}

