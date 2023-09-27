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
    : Widget(core), m_drawCursor(false), m_cursorTimer(nullptr), m_trailing(false),
      m_cursorPosition(0), m_lineHeight(0.0f), m_text(nullptr), m_placeholder(nullptr)
{
    m_text = new Text(core, "");
    m_text->setAlignment(Alignment::LEFT);
    m_cutoffText = new Text(core, "");
    m_cutoffText->setAlignment(Alignment::LEFT);
    m_type = WidgetType::TEXTEDIT;
    ColorTheme::createDefault(m_theme, m_type);
    m_drawBackground = true;
    m_drawBorder = true;
    m_hoverable = true;
    m_selectable = true;
    m_size = { 120.0f, 20.0f };
    m_cursorTimer = new Timer(m_core, 500, [this] { switchCursor(); });
}

Textedit::~Textedit()
{
    delete m_text;
    delete m_cursorTimer;
}

void Textedit::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_RECT_F rect = DPIHandler::adjusted(currentRect());
    basicDrawBackgroundBorder(rect, renderTarget, brush);
    brush->SetColor(Color::d2d1(m_theme->text[(int)m_state]));
    m_text->draw(rect, renderTarget, brush);

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

    if (m_placeholder && !m_selected && m_charWidths.empty())
    {
        brush->SetColor(Color::d2d1(m_theme->addColor[(int)m_state]));
        m_placeholder->draw(rect, renderTarget, brush);
    }
    if (m_drawCursor)
    {
        brush->SetColor(Color::d2d1(m_theme->border2[(int)m_state]));
        D2D1_RECT_F cursor = currentCursorLine();
        renderTarget->DrawLine(DPIHandler::adjusted(D2D1_POINT_2F{ cursor.left, cursor.top }),
            DPIHandler::adjusted(D2D1_POINT_2F{ cursor.right, cursor.bottom }), brush, m_theme->width2);
    }
}

void Textedit::place(int x, int y)
{
    basicPlace(x, y);
}

void Textedit::clicked(float mouseX, float mouseY)
{
    if (!m_cursorTimer->isActive())
    {
        m_cursorTimer->start();
    }

    uint32_t newCursorPos = getCursorPosition(mouseX);
    if (newCursorPos != m_cursorPosition)
    {
        restartCursorTimer();
    }
    m_cursorPosition = newCursorPos;
}

void Textedit::stopCursorTimer()
{
    m_cursorTimer->stop();
    m_drawCursor = false;
}

std::string Textedit::getText() const
{
    return m_text->getText();
}

Text* Textedit::getTextWidget(const std::string& which)
{
    if (which == "main")
    {
        return m_text;
    }
    else if (which == "placeholder")
    {
        return m_placeholder;
    }
    return nullptr;
}

void Textedit::setPlaceholderText(const std::string& text)
{
    if (!m_placeholder)
    {
        m_placeholder = new Text(m_core, "");
    }
    m_placeholder->setText(text);
    m_placeholder->setAlignment(Alignment::LEFT);
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

    D2D1_RECT_F rect = currentRect();
    if (std::accumulate(m_charWidths.begin(), m_charWidths.begin(), 0.0) > rect.right - rect.left - 2.0f)
    {

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
    getWriteFactory(m_core)->CreateTextLayout(&c, 1, m_text->getFontFormat(), 1.0f, 1.0f, &layout);
    DWRITE_TEXT_METRICS metrics;
    layout->GetMetrics(&metrics);
    safeReleaseInterface(&layout);

    if (m_lineHeight < metrics.height)
    {
        m_lineHeight = metrics.height;
    }
    return metrics.widthIncludingTrailingWhitespace;
}

uint32_t Textedit::getCursorPosition(float x) const
{
    std::string& text = m_text->getText();
    if (text.empty())
    {
        return 0;
    }

    float width = currentRect().left + 2.0f;
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
    D2D1_RECT_F rect = currentRect();
    float yGap = (rect.bottom - rect.top - m_lineHeight) / 2.0f;
    float xGap = std::accumulate(m_charWidths.begin(), m_charWidths.begin() + m_cursorPosition, 0.0f);
    yGap = limitRange(yGap, 0.0f, 1e6f);
    xGap = limitRange(xGap, 0.0f, rect.right - rect.left - 4.0f);
    return { rect.left + xGap + 2.0f, rect.top + yGap,
             rect.left + xGap + 2.0f, rect.top + yGap + m_lineHeight };
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
