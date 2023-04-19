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

void Label::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush, ID2D1StrokeStyle* strokeStyle)
{
    D2D1_COLOR_F background = toD2DColorF(m_theme.bg);
    D2D1_COLOR_F border = toD2DColorF(m_theme.brd);
    D2D1_COLOR_F text = toD2DColorF(m_theme.txt);
    D2D1_RECT_F rectangle = DPIHandler::adjusted(currentRect());

    if (m_drawBackground)
    {
        brush->SetColor(background);
        renderTarget->FillRectangle(rectangle, brush);
    }
    if (m_drawBorder)
    {
        brush->SetColor(border);
        renderTarget->DrawRectangle(rectangle, brush);
    }

    if (!m_fontFormat)
    {
        if (!createFontFormat())
        {
            throw std::runtime_error("Could not create Font Format");
            return;
        }
    }

    brush->SetColor(text);
    renderTarget->DrawText(toWideString(m_text).c_str(), (uint32_t)m_text.size(), m_fontFormat, rectangle, brush);
}

void Label::place(int x, int y)
{
    show();
    m_point = D2D1::Point2F(DPIHandler::PixelsToDips((float)x), DPIHandler::PixelsToDips((float)y));
}
