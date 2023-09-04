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
}

Label::~Label()
{
    delete m_text;
}

void Label::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    drawBasicShape(renderTarget, brush);
    D2D1_RECT_F rectangle = DPIHandler::adjusted(currentRect());
    m_text->draw(rectangle, renderTarget, brush);
}

void Label::place(int x, int y)
{
    basicPlace(x, y);
}

void Label::setText(const std::string& text)
{
    m_text->setText(text);
}
