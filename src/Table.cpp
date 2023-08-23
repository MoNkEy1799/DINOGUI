#include "Dinogui.h"
#include "Utils.h"

using namespace DINOGUI;

Table::Table(Core* core)
	: Widget(core), m_rows(0), m_cols(0)
{
    m_type = WidgetType::TABLE;
    m_drawBackground = true;
    m_drawBorder = true;
    m_size = { 120.0f, 20.0f };
}

Table::~Table()
{
}

void Table::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    drawBasicShape(renderTarget, brush);
    D2D1_COLOR_F colText = Color::d2d1(m_theme.txt);
    D2D1_RECT_F rectangle = DPIHandler::adjusted(currentRect());

    if (!m_fontFormat)
    {
        throwIfFailed(createFontFormat(), "Failed to create text format");
    }

    for (int row = 0; row < m_rows; row++)
    {
        for (int col = 0; col < m_cols; col++)
        {
            //render white boxes on gray background
            // --> leave gaps of 1 px to make it look like dividing lines
        }
    }

    brush->SetColor(colText);
    renderTarget->DrawText(toWideString(m_text).c_str(), (uint32_t)m_text.size(), m_fontFormat, rectangle, brush);
}

void Table::place(int x, int y)
{
    basicPlace(x, y);
}