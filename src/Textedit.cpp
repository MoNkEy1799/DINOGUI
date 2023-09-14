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
    : Widget(core), m_selected(false), m_drawCursor(false), m_cursorTimer(nullptr), m_trailing(false),
      m_cursorPosition(0), m_lineHeight(0.0f), m_text(nullptr), m_placeholder(nullptr)
{
    m_text = new Text(core, "");
    m_text->setAlignment(Alignment::LEFT);
    m_cutoffText = new Text(core, "");
    m_cutoffText->setAlignment(Alignment::LEFT);
    m_type = WidgetType::TEXTEDIT;
    m_drawBackground = true;
    m_drawBorder = true;
    m_size = { 120.0f, 20.0f };
    m_cursorTimer = new Timer(m_core->getWindowHandle(), 500, [this] { switchCursor(); });
    m_theme->background = { Color{ 255, 255, 255 }, Color{ 255, 255, 255 }, Color{ 255, 255, 255 } };
}

Textedit::~Textedit()
{
    delete m_text;
    delete m_cursorTimer;
}

void Textedit::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_RECT_F rect = DPIHandler::adjusted(currentRect());
    m_theme->border = { Color{ 51, 51, 51 }, Color{ 0, 120, 215 }, Color{ 51, 51, 51 } };
    if (m_selected)
    {
        m_theme->border = { Color{ 1, 86, 155 }, Color{ 1, 86, 155 }, Color{ 1, 86, 155 } };
    }
    basicDrawBackgroundBorder(rect, renderTarget, brush);
    m_text->setColor(m_theme->text[(int)m_state]);
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
        m_placeholder->setColor(m_theme->texteditPlaceholder[(int)m_state]);
        m_placeholder->draw(rect, renderTarget, brush);
    }
    if (m_drawCursor)
    {
        brush->SetColor(Color::d2d1(m_theme->texteditCursor[(int)m_state]));
        D2D1_RECT_F cursor = currentCursorLine();
        renderTarget->DrawLine(DPIHandler::adjusted(D2D1_POINT_2F{ cursor.left, cursor.top }),
            DPIHandler::adjusted(D2D1_POINT_2F{ cursor.right, cursor.bottom }), brush);
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

std::string Textedit::getText() const
{
    return m_text->getText();
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
    yGap = std::max(yGap, 0.0f);
    xGap = std::min(xGap, rect.right - rect.left - 4.0f);
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
