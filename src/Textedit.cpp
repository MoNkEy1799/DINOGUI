#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <string>

using namespace DINOGUI;

Textedit::Textedit(Core* core)
    : Widget(core), m_selected(false)
{
    m_type = WidgetType::TEXTEDIT;
    m_drawBackground = true;
    m_drawBorder = true;
}

void Textedit::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush, ID2D1StrokeStyle* strokeStyle)
{
    D2D1_COLOR_F background;
    D2D1_COLOR_F border;
    D2D1_COLOR_F text;
    D2D1_RECT_F rectangle = drawingAdjusted(currentRect());

    switch (m_state)
    {
    case WidgetState::NORMAL:
        background = toD2DColorF(Color(1.0f, 1.0f, 1.0f));
        border = toD2DColorF(Color(0.2f, 0.2f, 0.2f));
        text = toD2DColorF(m_theme.txt);
        break;

    case WidgetState::HOVER:
        background = toD2DColorF(Color(1.0f, 1.0f, 1.0f));
        border = toD2DColorF(m_theme.brd_h);
        text = toD2DColorF(m_theme.txt_h);
        break;

    case WidgetState::CLICKED:
        background = toD2DColorF(Color(1.0f, 1.0f, 1.0f));
        border = toD2DColorF(m_theme.brd_c);
        text = toD2DColorF(m_theme.txt_c);
        break;
    }

    if (m_selected)
    {
        background = toD2DColorF(Color(1.0f, 1.0f, 1.0f));
        border = toD2DColorF(m_theme.brd_c);
        text = toD2DColorF(m_theme.txt);
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

void Textedit::place(int x, int y)
{
    show();
    m_point = D2D1::Point2F(DPIConverter::PixelsToDips(x), DPIConverter::PixelsToDips(y));
}

void Textedit::clicked()
{
    m_selected = !m_selected;
}
