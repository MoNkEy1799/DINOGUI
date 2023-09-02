#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <vector>
#include <numeric>

using namespace DINOGUI;

Textedit::Textedit(Core* core)
    : Widget(core), m_selected(false), m_drawCursor(false), m_cursorTimer(nullptr),
      m_cursorPosition(0), m_lineHeight(0.0f), m_text(nullptr)
{
    m_text = new Text(core, "");
    m_text->setHorizontalAlignment(H_TextAlignment::LEADING);
    m_type = WidgetType::TEXTEDIT;
    m_drawBackground = true;
    m_drawBorder = true;
    m_size = { 120.0f, 20.0f };
    m_cursorTimer = new Timer(m_core->getWindowHandle(), 500, [this] { switchCursor(); });
}

Textedit::~Textedit()
{
    delete m_text;
    delete m_cursorTimer;
}

void Textedit::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    m_theme.bg = Color{ 255, 255, 255 };
    m_theme.bg_h = Color{ 255, 255, 255 };
    m_theme.bg_c = Color{ 255, 255, 255 };
    m_theme.brd = Color{ 51, 51, 51 };
    if (m_selected)
    {
        m_theme.brd = Color{ 1, 86, 155 };
    }
    drawBasicShape(renderTarget, brush);
    D2D1_RECT_F textRect = DPIHandler::adjusted(currentTextRect());
    m_text->draw(textRect, renderTarget, brush);

    if (m_text->fontFormatChanged)
    {
        m_text->fontFormatChanged = false;
        calculateCharDimension('A');

        std::string& text = m_text->getText();
        if (text.empty())
        {
            m_charWidths.clear();
            for (char c : text)
            {
                m_charWidths.push_back(calculateCharDimension(c));
            }
        }
    }
    
    if (m_drawCursor)
    {
        brush->SetColor(Color::d2d1(m_theme.txt));
        D2D1_RECT_F rec = currentCursorLine();
        renderTarget->DrawLine(DPIHandler::adjusted(D2D1_POINT_2F{ rec.left, rec.top }),
            DPIHandler::adjusted(D2D1_POINT_2F{ rec.right, rec.bottom }), brush);
    }
}

void Textedit::place(int x, int y)
{
    basicPlace(x, y);
}

void Textedit::clicked(float mouseX, float mouseY)
{
    if (!m_selected)
    {
        m_selected = true;
        m_cursorTimer->start();
    }

    uint32_t newCursorPos = getCursorPosition(mouseX);
    if (newCursorPos != m_cursorPosition)
    {
        restartCursorTimer();
    }
    m_cursorPosition = newCursorPos;
}

void Textedit::unselect()
{
    m_selected = false;
    m_drawCursor = false;
    m_cursorTimer->stop();
    m_state = WidgetState::NORMAL;
    m_core->setSelectedWidget(nullptr);
    m_core->redrawScreen();
}

std::string Textedit::getText()
{
    return m_text->getText();
}

void Textedit::keyInput(char key)
{
    std::string& text = m_text->getText();
    if (key == VK_BACK)
    {
        if (text.empty() || m_cursorPosition == 0)
        {
            return;
        }
        text.erase(m_cursorPosition - 1, 1);
        m_charWidths.erase(m_charWidths.begin() + m_cursorPosition - 1);
        updateCursorPosition(false);
    }
    else
    {
        text.insert(m_cursorPosition, 1, key);
        m_charWidths.insert(m_charWidths.begin() + m_cursorPosition, calculateCharDimension(key));
        updateCursorPosition(true);
    }
    m_core->redrawScreen();
}

void Textedit::otherKeys(uint32_t key)
{
    std::string& text = m_text->getText();
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
        if (text.empty() || m_cursorPosition == (uint32_t)text.size())
        {
            return;
        }
        text.erase(m_cursorPosition, 1);
        m_charWidths.erase(m_charWidths.begin() + m_cursorPosition);
        restartCursorTimer();
    }
}

float Textedit::calculateCharDimension(char character)
{
    IDWriteTextLayout* layout;
    const wchar_t c = (wchar_t)character;
    m_core->getWriteFactory()->CreateTextLayout(&c, 1, m_text->getFontFormat(), 1.0f, 1.0f, &layout);
    DWRITE_TEXT_METRICS metrics;
    layout->GetMetrics(&metrics);
    safeReleaseInterface(&layout);

    if (m_lineHeight < metrics.height)
    {
        m_lineHeight = metrics.height;
    }
    return metrics.widthIncludingTrailingWhitespace;
}

uint32_t Textedit::getCursorPosition(float x)
{
    std::string& text = m_text->getText();
    if (text.empty())
    {
        return 0;
    }

    float width = currentTextRect().left;
    size_t pos = 0;
    while (pos < text.size())
    {
        if (x <= width + m_charWidths[pos] / 2)
        {
            return (uint32_t)pos;
        }
        width += m_charWidths[pos];
        pos++;
    }

    return (uint32_t)text.size();
}

void Textedit::updateCursorPosition(bool increase)
{
    std::string& text = m_text->getText();
    if (increase && m_cursorPosition < (uint32_t)text.size())
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
    float yGap = (textRect.bottom - textRect.top - m_lineHeight) / 2.0f;
    float xGap = std::accumulate(m_charWidths.begin(), m_charWidths.begin() + m_cursorPosition, 0.0f);
    xGap = std::min(xGap, textRect.right - textRect.left);
    return { textRect.left + xGap, textRect.top + yGap,
             textRect.left + xGap, textRect.top + yGap + m_lineHeight };
}

D2D1_RECT_F Textedit::currentTextRect() const
{
    D2D1_RECT_F current = currentRect();
    return { current.left + 2.0f, current.top, current.right - 2.0f, current.bottom };
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
