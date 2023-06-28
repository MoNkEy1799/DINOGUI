#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Label::Label(Core* core, const std::string& text)
    : Widget(core)
{
	m_type = WidgetType::LABEL;
    m_text = text;
}

void Label::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    drawBasicShape(renderTarget, brush);
    D2D1_COLOR_F colText = Color::d2d1(m_theme.txt);
    D2D1_RECT_F rectangle = DPIHandler::adjusted(currentRect());

    if (!m_fontFormat)
    {
        throwIfFailed(createFontFormat(), "Failed to create text format");
    }

    brush->SetColor(colText);
    renderTarget->DrawText(toWideString(m_text).c_str(), (uint32_t)m_text.size(), m_fontFormat, rectangle, brush);
}

void Label::place(int x, int y)
{
    basicPlace(x, y);
}
