#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

using namespace DINOGUI;

Combobox::Combobox(Core* core, const std::string& text)
	: Widget(core), m_boxText({ nullptr }), m_currentIndex(0), m_hoverIndex(-1), m_dropdown(false)
{
    m_boxText[0] = new Text(core, text);
    m_boxText[0]->setAlignment(Alignment::LEFT);
    m_type = WidgetType::COMBOBOX;
    m_drawBackground = true;
    m_drawBorder = true;
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
    drawBasicShape(renderTarget, brush);
    D2D1_RECT_F rect = currentRect();
    rect = { rect.left, rect.top, rect.right - 16.0f, rect.bottom };
    std::array<D2D1_POINT_2F, 3> points = getArrowPoints();
    brush->SetColor(Color::d2d1(DINOCOLOR_BLACK));
    renderTarget->DrawLine(DPIHandler::adjusted(points[0]), DPIHandler::adjusted(points[1]), brush);
    renderTarget->DrawLine(DPIHandler::adjusted(points[1]), DPIHandler::adjusted(points[2]), brush);
    m_boxText[m_currentIndex]->draw(rect, renderTarget, brush);

    if (m_dropdown)
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
    m_dropdown = true;
}

void Combobox::unselect()
{
    m_dropdown = false;
}

bool Combobox::dropdownContains(float x, float y) const
{
    if (indexUnderMouse(x, y) == -1)
    {
        return false;
    }
    return true;
}

void Combobox::setHoverIndex(int index)
{
    m_hoverIndex = index;
}

void Combobox::addItem(const std::string& text)
{
    m_boxText.push_back(new Text(m_core, text));
}

void Combobox::insertItem(const std::string& text, int index)
{
    m_boxText.insert(m_boxText.begin() + index, new Text(m_core, text));
}

void Combobox::removeItem(int index)
{
    if (index >= m_boxText.size())
    {
        return;
    }
    m_boxText.erase(m_boxText.begin() + index);
}

std::string Combobox::getCurrentText() const
{
    return m_boxText[m_currentIndex]->getText();
}

int Combobox::getCurrentIndex() const
{
    return m_currentIndex;
}

std::array<D2D1_POINT_2F, 3> Combobox::getArrowPoints() const
{
    D2D1_RECT_F rect = currentRect();
    rect = { rect.right - 16.0f, rect.top, rect.right, rect.bottom };
    float midWidth = rect.left + (rect.right - rect.left) / 2.0f;
    float midHeight = rect.top + (rect.bottom - rect.top) / 2.0f;
    return { {{ midWidth - 4.0f, midHeight - 1.0f }, { midWidth, midHeight + 3.0f }, { midWidth + 5.0f, midHeight - 2.0f }} };
}

int Combobox::indexUnderMouse(float x, float y) const
{
    for (int i = 0; i < m_boxText.size(); i++)
    {
        if (contains(x, y - m_size.height * (i + 1)))
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
    rect = { rect.left, rect.bottom, rect.right, rect.bottom + m_boxText.size() * m_size.height };
    brush->SetColor(Color::d2d1(DINOCOLOR_WHITE));
    renderTarget->FillRectangle(DPIHandler::adjusted(rect), brush);
    brush->SetColor(Color::d2d1(m_theme.brd));
    renderTarget->DrawRectangle(DPIHandler::adjusted(rect), brush);

    D2D1_RECT_F boxRect = currentRect();
    for (int i = 0; i < m_boxText.size(); i++)
    {
        boxRect.top += m_size.height;
        boxRect.bottom += m_size.height;
        if (m_hoverIndex == i)
        {
            brush->SetColor(Color::d2d1(m_theme.brd_h));
            renderTarget->FillRectangle(DPIHandler::adjusted(boxRect), brush);
        }
        m_boxText[i]->draw(boxRect, renderTarget, brush);
    }
}
