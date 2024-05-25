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
    : Widget(core), m_drawCursor(false), m_cursorTimer(nullptr), m_leftbound(true),
      m_cursorPosition(0), m_selectionCursor(0), m_cutoffPosition(0), m_lineHeight(0.0f),
      m_text(nullptr), m_placeholder(nullptr)
{
    m_text = new Text(core, "");
    m_text->setAlignment(Alignment::LEFT);
    m_type = WidgetType::TEXTEDIT;
    ColorTheme::createDefault(m_theme, m_type);
    m_drawBackground = true;
    m_drawBorder = true;
    m_hoverable = true;
    m_selectable = true;
    m_holdable = true;
    m_size = { 100.0f, 20.0f };
    m_cursorTimer = new Timer(m_core, 500, [this] { switchCursor(); });
}

Textedit::~Textedit()
{
    delete m_text;
    delete m_placeholder;
    delete m_cursorTimer;
}

void Textedit::draw()
{
    D2D1_RECT_F rect = DPIHandler::adjusted(currentRect());
    basicDrawBackgroundBorder(rect);
    ID2D1HwndRenderTarget* renderTarget = getRenderTarget(m_core);
    ID2D1SolidColorBrush* brush = getColorBrush(m_core);
    std::pair<uint32_t, uint32_t> cursor = cursorOrder();

    if (m_leftbound)
    {
        if (m_cursorPosition < m_cutoffPosition)
        {
            m_cutoffPosition = m_cursorPosition;
        }
        else if (addCharWidths(m_cutoffPosition, m_cursorPosition) > rect.right - rect.left - 5.0f)
        {
            m_leftbound = false;
            m_cutoffPosition = m_cursorPosition;
            m_text->setAlignment(Alignment::RIGHT);
        }
    }
    else
    {
        if (m_cursorPosition > m_cutoffPosition)
        {
            m_cutoffPosition = m_cursorPosition;
        }
        else if (addCharWidths(m_cursorPosition, m_cutoffPosition) > rect.right - rect.left - 5.0f)
        {
            m_leftbound = true;
            m_cutoffPosition = m_cursorPosition;
            m_text->setAlignment(Alignment::LEFT);
        }
    }

    if (cursor.first != cursor.second)
    {
        D2D1_RECT_F select = DPIHandler::adjusted(selectionRect());
        brush->SetColor(Color::d2d1(m_theme->background2[(int)m_state]));
        renderTarget->FillRectangle(select, brush);
    }
    brush->SetColor(Color::d2d1(m_theme->text[(int)m_state]));
    m_text->draw(rect, 0, -1, 1.0f);
    if (cursor.first != cursor.second)
    {
        brush->SetColor(Color::d2d1(m_theme->text2[(int)m_state]));
        rect.left += addCharWidths(0, cursor.first);
        m_text->draw(rect, cursor.first, cursor.second, 1.0f);
    }

    if (m_text->fontFormatChanged)
    {
        m_text->fontFormatChanged = false;
        calculateCharDimension('A');
        std::string& text = m_text->getText();
        if (!text.empty())
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
        m_placeholder->draw(rect);
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

void Textedit::clicked(float mouseX, float mouseY, bool hold)
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
    if (hold)
    {
        m_cursorPosition = newCursorPos;
        return;
    }
    m_selectionCursor = newCursorPos;
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
        if (text.empty())
        {
            return;
        }
        if ((m_cursorPosition == m_selectionCursor) && m_cursorPosition != 0)
        {
            text.erase(m_cursorPosition - 1, 1);
            m_charWidths.erase(m_charWidths.begin() + m_cursorPosition - 1);
            updateCursorPosition(false);
            return;
        }
        std::pair<uint32_t, uint32_t> cursor = cursorOrder();
        text.erase(cursor.first, cursor.second - cursor.first);
        m_cursorPosition = cursor.first;
        m_selectionCursor = cursor.first;
    }
    else
    {
        std::pair<uint32_t, uint32_t> cursor = cursorOrder();
        if (cursor.first != cursor.second)
        {
            text.erase(cursor.first, cursor.second - cursor.first);
            m_cursorPosition = cursor.first;
            m_selectionCursor = cursor.first;
        }
        text.insert(m_cursorPosition, 1, key);
        m_charWidths.insert(m_charWidths.begin() + m_cursorPosition, calculateCharDimension(key));
        updateCursorPosition(true);
    }

    m_core->redrawScreen();
}

void Textedit::otherKeys(uint32_t key, const std::vector<uint32_t>& pressed)
{
    std::string& text = m_text->getText();
    if (key == VK_LEFT)
    {
        if (m_cursorPosition != m_selectionCursor)
        {
            std::pair<uint32_t, uint32_t> cursor = cursorOrder();
            m_cursorPosition = cursor.first;
            m_selectionCursor = cursor.first;
        }
        else
        {
            updateCursorPosition(false);
        }
    }
    else if (key == VK_RIGHT)
    {
        if (m_cursorPosition != m_selectionCursor)
        {
            std::pair<uint32_t, uint32_t> cursor = cursorOrder();
            m_cursorPosition = cursor.second;
            m_selectionCursor = cursor.second;
        }
        else
        {
            updateCursorPosition(true);
        }
    }
    else if (key == VK_DELETE)
    {
        if (text.empty())
        {
            return;
        }
        if ((m_cursorPosition == m_selectionCursor) && m_cursorPosition != (uint32_t)text.length())
        {
            text.erase(m_cursorPosition, 1);
            m_charWidths.erase(m_charWidths.begin() + m_cursorPosition);
            restartCursorTimer();
        }
        else
        {
            std::pair<uint32_t, uint32_t> cursor = cursorOrder();
            text.erase(cursor.first, cursor.second - cursor.first);
            m_cursorPosition = cursor.first;
            m_selectionCursor = cursor.first;
        }
    }
    D2D1_RECT_F rect = currentRect();
}

float Textedit::calculateCharDimension(char character)
{
    IDWriteTextLayout* layout;
    const wchar_t c = (wchar_t)character;
    getWriteFactory(m_core)->CreateTextLayout(&c, 1, m_text->getFontFormat(), 1.0f, 1.0f, &layout);
    DWRITE_TEXT_METRICS metrics;
    layout->GetMetrics(&metrics);
    safeReleaseInterface(&layout);
    m_lineHeight = metrics.height;
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
    while (pos < text.length())
    {
        if (x <= width + m_charWidths[pos] / 2)
        {
            return (uint32_t)pos;
        }
        width += m_charWidths[pos];
        pos++;
    }

    return (uint32_t)text.length();
}

float Textedit::addCharWidths(uint32_t start, uint32_t end) const
{
    return std::accumulate(m_charWidths.begin() + start, m_charWidths.begin() + end, 0.0f);
}

void Textedit::updateCursorPosition(bool increase)
{
    if (increase && m_cursorPosition < (uint32_t)m_text->getText().length())
    {
        m_cursorPosition++;
        m_selectionCursor++;
        restartCursorTimer();
    }
    else if (!increase && m_cursorPosition > 0)
    {
        m_cursorPosition--;
        m_selectionCursor--;
        restartCursorTimer();
    }
}

std::pair<uint32_t, uint32_t> Textedit::cursorOrder() const
{
    uint32_t first = m_cursorPosition;
    uint32_t second = m_selectionCursor;
    if (m_selectionCursor < m_cursorPosition)
    {
        first = m_selectionCursor;
        second = m_cursorPosition;
    }
    return { first, second };
}

D2D1_RECT_F Textedit::currentCursorLine() const
{
    D2D1_RECT_F rect = currentRect();
    std::cout << m_cutoffPosition << " | " << m_cursorPosition << std::endl;
    float xGap = addCharWidths(m_cutoffPosition, m_cursorPosition);
    float yGap = (rect.bottom - rect.top - m_lineHeight) / 2.0f;
    yGap = limitRange(yGap, 0.0f, 1e6f);
    xGap = limitRange(xGap, 0.0f, rect.right - rect.left - 5.0f);
    float side = rect.left + 2.0f;
    if (!m_leftbound)
    {
        side = rect.right - 3.0f;
        xGap *= -1;
    }
    std::cout << side << " # " << xGap << std::endl;
    return { side + xGap, rect.top + yGap, side + xGap, rect.bottom - yGap };
}

D2D1_RECT_F Textedit::selectionRect() const
{
    std::pair<uint32_t, uint32_t> cursor = cursorOrder();
    D2D1_RECT_F rect = currentRect();
    float yGap = (rect.bottom - rect.top - m_lineHeight) / 2.0f;
    float xGap = addCharWidths(0, cursor.first);
    float width = addCharWidths(cursor.first, cursor.second);
    yGap = limitRange(yGap, 0.0f, 1e6f);
    xGap = limitRange(xGap, 0.0f, rect.right - rect.left - 4.0f);
    return { rect.left + xGap + 2.0f, rect.top + yGap, rect.left + xGap + width + 3.0f, rect.bottom - yGap + 1.0f };
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
