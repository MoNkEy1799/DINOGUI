#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <string>

using namespace DINOGUI;

Textedit::Textedit(Core* core)
    : Widget(core), m_selected(false), m_drawCursor(false),
      m_cursorPoint({ 0.0f, 0.0f }), m_cursorSize({ 0.0f, 0.0f })
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
    D2D1_RECT_F textRect = drawingAdjusted(currentTextRect());

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
    if (m_drawCursor)
    {
        brush->SetColor(toD2DColorF(m_theme.txt));
        D2D1_RECT_F rec = drawingAdjusted(currentCursorLine());
        renderTarget->DrawLine({ rec.left, rec.top }, { rec.right, rec.bottom }, brush);
    }

    if (!m_fontFormat)
    {
        if (!createFontFormat())
        {
            throw std::runtime_error("Could not create Font Format");
            return;
        }
        if (FAILED(m_fontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)))
        {
            throw std::runtime_error("Could not align Font Format");
        }
    }

    brush->SetColor(text);
    renderTarget->DrawText(toWideString(m_text).c_str(), (uint32_t)m_text.size(), m_fontFormat, textRect, brush);
}

void Textedit::place(int x, int y)
{
    show();
    m_point = D2D1::Point2F(DPIConverter::PixelsToDips((float)x), DPIConverter::PixelsToDips((float)y));
    m_cursorPoint = { m_point.x + 2.0f, m_point.y + 4.0f };
    m_cursorSize = { 0.0f, 12.0f };
}

void Textedit::clicked()
{
    m_selected = !m_selected;
    if (m_selected)
    {
        SetTimer(m_core->getWindowHandle(), (uint64_t)this, 500, (TIMERPROC)Textedit::switchCursor);
    }
    else
    {
        m_drawCursor = false;
        KillTimer(m_core->getWindowHandle(), (uint64_t)this);
    }
}

void Textedit::keyInput(char key)
{
    if (key == VK_BACK)
    {
        if (m_text.empty())
        {
            return;
        }
        m_text.pop_back();
        m_cursorPoint.x -= 5.0f;
    }
    else
    {
        m_text.push_back(key);
        m_cursorPoint.x += 5.0f;
    }
    m_core->redrawScreen();
}

D2D1_RECT_F Textedit::currentCursorLine() const
{
    return { m_cursorPoint.x, m_cursorPoint.y, m_cursorPoint.x + m_cursorSize.width, m_cursorPoint.y + m_cursorSize.height };
}

D2D1_RECT_F Textedit::currentTextRect() const
{
    D2D1_RECT_F current = currentRect();
    return { current.left + 2.0f, current.top, current.right, current.bottom };
}

void Textedit::switchCursor(HWND, uint32_t, uint64_t classPtr, DWORD)
{
    Textedit* self = (Textedit*)classPtr;
    self->m_drawCursor = !(self->m_drawCursor);
    self->m_core->redrawScreen();
}
