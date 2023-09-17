#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Button::Button(Core* core, const std::string& text, std::function<void()> function)
    : Widget(core), m_clickFunction(function), m_text(nullptr)
{
    m_text = new Text(core, text);
    m_type = WidgetType::BUTTON;
    ColorTheme::createDefault(m_theme, m_type);
    m_drawBackground = true;
    m_drawBorder = true;
    m_hoverable = true;
    m_clickable = true;
}

Button::~Button()
{
    delete m_text;
}

void Button::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_RECT_F rect = DPIHandler::adjusted(currentRect());
    basicDrawBackgroundBorder(rect, renderTarget, brush);
    m_text->setColor(m_theme->text[(int)m_state]);
    m_text->draw(rect, renderTarget, brush);
}

void Button::place(int x, int y)
{
    basicPlace(x, y);
}

void Button::clicked(float mouseX, float mouseY)
{
    if (m_clickFunction)
    {
        m_clickFunction();
    }
}

void Button::connect(std::function<void()> function)
{
    m_clickFunction = function;
}

void Button::setText(const std::string& text)
{
    m_text->setText(text);
}

void Button::setCheckable(bool check)
{
    m_checkable = check;
    m_clickable = !check;
}

bool Button::isChecked()
{
    return m_checked;
}

