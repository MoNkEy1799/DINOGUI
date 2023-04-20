#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <vector>

using namespace DINOGUI;

Textedit::Textedit(Core* core)
    : Widget(core), m_selected(false), m_drawCursor(false), m_cursorTimer(nullptr),
      m_cursorPosition(0)
{
    m_type = WidgetType::TEXTEDIT;
    m_drawBackground = true;
    m_drawBorder = true;
    m_size = { 120.0f, 20.0f };
    m_cursorTimer = new Timer(m_core->getWindowHandle(), 500, [this] { switchCursor(); });
}

Textedit::~Textedit()
{
    delete m_cursorTimer;
}

void Textedit::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_COLOR_F background;
    D2D1_COLOR_F border;
    D2D1_COLOR_F text;
    D2D1_RECT_F rectangle = DPIHandler::adjusted(currentRect());
    D2D1_RECT_F textRect = DPIHandler::adjusted(currentTextRect());

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
        throwIfFailed(createFontFormat(), "Failed to create text format");
        throwIfFailed(m_fontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING), "Failed to align text format");

        std::array<float, 2> a = calculateCharDimension(L" ");
        std::cout << a[0] << "  " << a[1] << std::endl;
    }

    brush->SetColor(text);
    renderTarget->DrawText(toWideString(m_text).c_str(), (uint32_t)m_text.size(), m_fontFormat, textRect, brush);
    
    if (m_drawCursor)
    {
        brush->SetColor(toD2DColorF(m_theme.txt));
        D2D1_RECT_F rec = DPIHandler::adjusted(currentCursorLine());
        renderTarget->DrawLine({ rec.left, rec.top }, { rec.right, rec.bottom }, brush);
    }
}

void Textedit::place(int x, int y)
{
    show();
    m_point = D2D1::Point2F(DPIHandler::PixelsToDips((float)x), DPIHandler::PixelsToDips((float)y));
}

void Textedit::clicked()
{
    m_selected = !m_selected;
    if (m_selected)
    {
        m_cursorTimer->start();
    }
    else
    {
        m_drawCursor = false;
        m_cursorTimer->stop();
    }
}

void Textedit::keyInput(char key)
{
    if (key == VK_BACK)
    {
        if (m_text.empty() || m_cursorPosition == 0)
        {
            return;
        }
        m_text.erase(m_cursorPosition - 1, 1);
        updateCursorPosition(false);
    }
    else
    {
        m_text.insert(m_cursorPosition, 1, key);
        updateCursorPosition(true);
    }
    m_core->redrawScreen();
}

void Textedit::otherKeys(uint32_t key)
{
    if (key == VK_LEFT)
    {
        updateCursorPosition(false);
    }
    else if (key == VK_RIGHT)
    {
        updateCursorPosition(true);
    }
    else if (key == VK_DELETE)
    {
        if (m_text.empty() || m_cursorPosition == (uint32_t)m_text.size())
        {
            return;
        }
        m_text.erase(m_cursorPosition, 1);
        restartCursorTimer();
    }
}

std::array<float, 2> Textedit::calculateCharDimension(const wchar_t* character)
{
    IDWriteTextLayout* layout;
    m_core->getWriteFactory()->CreateTextLayout(character, 1, m_fontFormat, 1.0f, 1.0f, &layout);
    DWRITE_TEXT_METRICS metrics;
    layout->GetMetrics(&metrics);
    safeReleaseInterface(&layout);
    return { metrics.widthIncludingTrailingWhitespace, metrics.height };
}

void Textedit::updateCursorPosition(bool increase)
{
    if (increase && m_cursorPosition < (uint32_t)m_text.length())
    {
        m_cursorPosition++;
        restartCursorTimer();
    }
    else if (!increase && m_cursorPosition > 0)
    {
        m_cursorPosition--;
        restartCursorTimer();
    }
}

D2D1_RECT_F Textedit::currentCursorLine() const
{
    D2D1_RECT_F textRect = currentTextRect();
    /*float top = (textRect.bottom - textRect.top - m_charHeight) / 2.0f;
    return { textRect.left + (m_charWidth * m_cursorPosition), textRect.top + top,
             textRect.left + (m_charWidth * m_cursorPosition), textRect.top + top + m_charHeight };
             */
    return { 0 };
}

D2D1_RECT_F Textedit::currentTextRect() const
{
    D2D1_RECT_F current = currentRect();
    return { current.left + 2.0f, current.top, current.right, current.bottom };
}

void Textedit::switchCursor()
{
    m_drawCursor = !(m_drawCursor);
    m_core->redrawScreen();
}

void Textedit::restartCursorTimer()
{
    m_drawCursor = true;
    m_cursorTimer->restart();
    m_core->redrawScreen();
}
