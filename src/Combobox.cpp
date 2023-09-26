#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

using namespace DINOGUI;

Combobox::Combobox(Core* core, const std::string& text)
	: Widget(core), m_boxText({ nullptr }), m_currentIndex(0), m_hoverIndex(-1),
      m_dropdown(false), m_upward(false)
{
    m_boxText[0] = new Text(core, text);
    m_boxText[0]->setAlignment(Alignment::LEFT);
    m_type = WidgetType::COMBOBOX;
    ColorTheme::createDefault(m_theme, m_type);
    m_drawBackground = true;
    m_drawBorder = true;
    m_hoverable = true;
    m_selectable = true;
}

Combobox::~Combobox()
{
    for (Text* text : m_boxText)
    {
        delete text;
    }
}

void Combobox::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_RECT_F rect = DPIHandler::adjusted(currentRect());
    basicDrawBackgroundBorder(rect, renderTarget, brush);
    D2D1_RECT_F textRect = currentRect();
    textRect = { textRect.left, textRect.top, textRect.right - 16.0f, textRect.bottom };
    std::array<D2D1_POINT_2F, 3> points = getArrowPoints();
    brush->SetColor(Color::d2d1(m_theme->text[(int)m_state]));
    renderTarget->DrawLine(DPIHandler::adjusted(points[0]), DPIHandler::adjusted(points[1]), brush, m_theme->width2);
    renderTarget->DrawLine(DPIHandler::adjusted(points[1]), DPIHandler::adjusted(points[2]), brush, m_theme->width2);
    brush->SetColor(Color::d2d1(m_theme->text[(int)m_state]));
    m_boxText[m_currentIndex]->draw(textRect, renderTarget, brush);

    if (m_selected)
    {
        drawDropdown(renderTarget, brush);
    }
}

void Combobox::place(int x, int y)
{
    basicPlace(x, y);
}

void Combobox::clicked(float mouseX, float mouseY)
{
    if (m_dropdown)
    {
        m_selected = false;
        setSelectWidget(m_core, nullptr);
        if (m_hoverIndex != -1)
        {
            m_currentIndex = m_hoverIndex;
            m_state = WidgetState::NORMAL;
        }
    }
    m_dropdown = !m_dropdown;
}

bool Combobox::dropdownContains(float x, float y) const
{
    if (indexUnderMouse(x, y) == -1)
    {
        return false;
    }
    return true;
}

void Combobox::setHoverIndex(float x, float y)
{
    int index = indexUnderMouse(x, y);
    if (index != m_hoverIndex)
    {
        m_hoverIndex = index;
        m_core->redrawScreen();
    }
}

void Combobox::setDropdown(bool drop)
{
    m_dropdown = drop;
}

void Combobox::addItem(const std::string& text)
{
    Text* textWidget = new Text(m_core, text);
    textWidget->setAlignment(Alignment::LEFT);
    m_boxText.push_back(textWidget);
}

void Combobox::insertItem(const std::string& text, int index)
{
    Text* textWidget = new Text(m_core, text);
    textWidget->setAlignment(Alignment::LEFT);
    m_boxText.insert(m_boxText.begin() + index, textWidget);
}

void Combobox::removeItem(int index)
{
    if (index >= m_boxText.size())
    {
        return;
    }
    m_boxText.erase(m_boxText.begin() + index);
}

void Combobox::changeUnfoldDirection(bool upward)
{
    m_upward = true;
}

std::string Combobox::getCurrentText() const
{
    return m_boxText[m_currentIndex]->getText();
}

int Combobox::getCurrentIndex() const
{
    return m_currentIndex;
}

std::vector<Text*> Combobox::getTextWidgets()
{
    return m_boxText;
}

Text* Combobox::getTextWidget(int index)
{
    return m_boxText[index];
}

std::array<D2D1_POINT_2F, 3> Combobox::getArrowPoints() const
{
    D2D1_RECT_F rect = currentRect();
    rect = { rect.right - 16.0f, rect.top, rect.right, rect.bottom };
    float midWidth = std::floor(rect.left + (rect.right - rect.left) / 2.0f);
    float midHeight = std::floor(rect.top + (rect.bottom - rect.top) / 2.0f);
    return { {{ midWidth - 4.0f, midHeight - 1.0f }, { midWidth, midHeight + 3.0f }, { midWidth + 5.0f, midHeight - 2.0f }} };
}

int Combobox::indexUnderMouse(float x, float y) const
{
    int dir = m_upward ? 1 : -1;
    for (int i = 0; i < m_boxText.size(); i++)
    {
        if (contains(x, y + dir * m_size.height * (i + 1)))
        {
            return i;
        }
    }
    return -1;
}

void Combobox::drawDropdown(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    if (m_boxText.empty())
    {
        return;
    }

    D2D1_RECT_F rect = currentRect();
    int dir = m_upward ? -1 : 1;
    float y = m_upward ? rect.top : rect.bottom;
    rect = { rect.left, y, rect.right, y + dir * (int)m_boxText.size() * m_size.height };
    brush->SetColor(Color::d2d1(m_theme->addColor[(int)m_state]));
    renderTarget->FillRectangle(DPIHandler::adjusted(rect), brush);
    brush->SetColor(Color::d2d1(m_theme->border2[(int)m_state]));
    renderTarget->DrawRectangle(DPIHandler::adjusted(rect), brush, m_theme->width);

    D2D1_RECT_F boxRect = currentRect();
    for (int i = 0; i < m_boxText.size(); i++)
    {
        boxRect.top += dir * m_size.height;
        boxRect.bottom += dir * m_size.height;
        brush->SetColor(Color::d2d1(m_theme->text[(int)m_state]));
        if (m_hoverIndex == i)
        {
            brush->SetColor(Color::d2d1(m_theme->background2[(int)m_state]));
            renderTarget->FillRectangle(DPIHandler::adjusted(boxRect), brush);
            brush->SetColor(Color::d2d1(m_theme->text2[(int)m_state]));
        }
        m_boxText[i]->draw(DPIHandler::adjusted(boxRect), renderTarget, brush);
    }
}
