#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

using namespace DINOGUI;

Combobox::Combobox(Core* core, const std::string& text)
	: Widget(core), m_boxText({ nullptr })
{
    m_boxText[0] = new Text(core, text);
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
    D2D1_RECT_F rectangle = DPIHandler::adjusted(currentRect());
    m_boxText[0]->draw(rectangle, renderTarget, brush);
}

void Combobox::place(int x, int y)
{
    basicPlace(x, y);
}

void Combobox::clicked(float mouseX, float mouseY)
{

}

void Combobox::unselect()
{

}
