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
            brush->SetColor({ 255, 255, 255 });
        }
    }

    brush->SetColor(colText);
    renderTarget->DrawText(toWideString(m_text).c_str(), (uint32_t)m_text.size(), m_fontFormat, rectangle, brush);
}

void Table::place(int x, int y)
{
    basicPlace(x, y);
}

void Table::setCell(const std::string& text, int row, int col)
{
    if (row + 1 > m_rows)
    {
        for (int i = m_rows - 1; i < row; i++)
        {
            for (int j = 0; j < m_cols; j++)
            {
                m_entries.push_back("");
            }
        }
        m_rows = row + 1;
    }
    if (col + 1 > m_cols)
    {
        for (int j = m_cols; j < col + 1; j++)
        {
            for (int i = 0; i < m_rows; i++)
            {
                m_entries.insert(m_entries.begin() + i * j + j + i, 0);
            }
        }
        m_cols = col + 1;
    }
    m_entries[row * m_cols + col] = text;
}
