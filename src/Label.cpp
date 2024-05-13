#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Label::Label(Core* core, const std::string& text)
    : Widget(core), m_text(nullptr)
{
    m_text = new Text(core, text);
	m_type = WidgetType::LABEL;
    ColorTheme::createDefault(m_theme, m_type);
}

Label::~Label()
{
    delete m_text;
}

void Label::draw()
{
    D2D1_RECT_F rect = DPIHandler::adjusted(currentRect());
    basicDrawBackgroundBorder(rect);
    getColorBrush(m_core)->SetColor(Color::d2d1(m_theme->text[(int)m_state]));
    m_text->draw(rect);
}

void Label::place(int x, int y)
{
    basicPlace(x, y);
}

Text* Label::getTextWidget()
{
    return m_text;
}

void Label::setText(const std::string& text)
{
    m_text->setText(text);
}
