#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

using namespace DINOGUI;

Table::Table(Core* core)
	: Widget(core), m_rows(1), m_cols(1), m_lineWidth(1.0f), m_entries({""})
{
    m_type = WidgetType::TABLE;
    m_drawBackground = true;
    m_drawBorder = true;
    m_size = { 120.0f, 140.0f };
}

void Table::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    m_theme.bg = Color{ 255, 255, 255 };
    m_theme.brd = Color{ 51, 51, 51 };
    drawBasicShape(renderTarget, brush);

    if (!m_fontFormat)
    {
        throwIfFailed(createFontFormat(), "Failed to create text format");
    }
    
    m_colWidth = std::floor((m_size.width - (m_cols - 1) * m_lineWidth) / m_cols);
    m_rowHeight = std::floor((m_size.height - (m_rows - 1) * m_lineWidth) / m_rows);
    for (int row = 0; row < m_rows; row++)
    {
        for (int col = 0; col < m_cols; col++)
        {
            drawTextInCell(row, col, renderTarget, brush);
        }
    }
    drawCellLines(renderTarget, brush);
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
                m_entries.insert(m_entries.begin() + i * j + j + i, "");
            }
        }
        m_cols = col + 1;
    }
    m_entries[row * m_cols + col] = text;
}

void Table::drawTextInCell(int row, int col, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_RECT_F cell = DPIHandler::adjusted({ m_point.x + (col + 1) * m_lineWidth + col * m_colWidth,
                         m_point.y + (row + 1) * m_lineWidth + row * m_rowHeight,
                         m_point.x + (col + 1) * m_lineWidth + (col + 1) * m_colWidth,
                         m_point.y + (row + 1) * m_lineWidth + (row + 1) * m_rowHeight });
    brush->SetColor(Color::d2d1(m_theme.txt));
    std::wstring text = toWideString(m_entries[row * m_cols + col]);
    renderTarget->DrawText(text.c_str(), (uint32_t)text.size(), m_fontFormat, cell, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void Table::drawCellLines(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_RECT_F line;
    float curHeight = m_point.y + m_rowHeight + m_lineWidth;
    float curWidth = m_point.x + m_colWidth + m_lineWidth;
    for (int row = 0; row < m_rows - 1; row++)
    {
        line = DPIHandler::adjusted({ m_point.x, curHeight, m_point.x + m_size.width, curHeight + m_lineWidth });
        DEBUG_PRINT_COORDS(line);
        brush->SetColor(Color::d2d1(DINOCOLOR_LIGHTGRAY));
        renderTarget->FillRectangle(line, brush);
        curHeight += m_rowHeight + m_lineWidth;
    }
    for (int col = 0; col < m_cols - 1; col++)
    {
        line = DPIHandler::adjusted({ curWidth, m_point.y, curWidth + m_lineWidth, m_point.y + m_size.height });
        brush->SetColor(Color::d2d1(DINOCOLOR_LIGHTGRAY));
        renderTarget->FillRectangle(line, brush);
        curWidth += m_colWidth + m_lineWidth;
    }
}
