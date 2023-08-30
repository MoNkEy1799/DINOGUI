#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

using namespace DINOGUI;

Table::Table(Core* core)
    : Widget(core), m_rows(1), m_cols(1), m_entries({ {"", 1, 1} }),
      m_lineWidth(1.0f), m_rowHeight(0.0f), m_colWidth(0.0f)
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
    
    m_colWidth = (m_size.width - (m_cols + 1) * m_lineWidth) / m_cols;
    m_rowHeight = (m_size.height - (m_rows + 1) * m_lineWidth) / m_rows;
    drawCellLines(renderTarget, brush);
    for (int row = 0; row < m_rows; row++)
    {
        for (int col = 0; col < m_cols; col++)
        {
            drawTextInCell(row, col, renderTarget, brush);
        }
    }
}

void Table::place(int x, int y)
{
    basicPlace(x, y);
}

void Table::setCell(const std::string& text, int row, int col, int rowSpan, int colSpan)
{
    if (row + 1 > m_rows)
    {
        for (int i = m_rows - 1; i < row; i++)
        {
            for (int j = 0; j < m_cols; j++)
            {
                m_entries.push_back({ "", 1, 1 });
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
                m_entries.insert(m_entries.begin() + i * j + j + i, { "", 1, 1 });
            }
        }
        m_cols = col + 1;
    }
    GridEntry<std::string>& entry = m_entries[row * m_cols + col];
    entry.entry = text;
    entry.rowSpan = rowSpan;
    entry.colSpan = colSpan;
}

void Table::setLineWidth(float lineWidth)
{
    m_lineWidth = lineWidth;
}

void Table::drawTextInCell(int row, int col, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    GridEntry<std::string>& entry = m_entries[row * m_cols + col];
    D2D1_RECT_F cell = DPIHandler::adjusted({ m_point.x + (col + 1) * m_lineWidth + col * m_colWidth,
                         m_point.y + (row + 1) * m_lineWidth + row * m_rowHeight,
                         m_point.x + (col + 1) * m_lineWidth + (col + 1 + entry.colSpan) * m_colWidth,
                         m_point.y + (row + 1) * m_lineWidth + (row + 1 + entry.rowSpan) * m_rowHeight });
    brush->SetColor(Color::d2d1(m_theme.bg));
    renderTarget->FillRectangle(cell, brush);
    brush->SetColor(Color::d2d1(m_theme.txt));
    std::wstring text = toWideString(entry.entry);
    renderTarget->DrawText(text.c_str(), (uint32_t)text.size(), m_fontFormat, cell, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void Table::drawCellLines(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_POINT_2F p1, p2;
    float curHeight = m_point.y + m_rowHeight + m_lineWidth;
    float curWidth = m_point.x + m_colWidth + m_lineWidth;
    for (int row = 0; row < m_rows - 1; row++)
    {
        p1 = { m_point.x + 1.0f, curHeight };
        p2 = { m_point.x + m_size.width - 0.5f, curHeight };
        brush->SetColor(Color::d2d1(DINOCOLOR_LIGHTGRAY));
        renderTarget->DrawLine(DPIHandler::adjusted(p1), DPIHandler::adjusted(p2), brush, m_lineWidth);
        DEBUG_PRINT(curHeight, row);
        curHeight += m_rowHeight + m_lineWidth;
    }
    for (int col = 0; col < m_cols - 1; col++)
    {
        p1 = { curWidth, m_point.y + 1.0f };
        p2 = { curWidth, m_point.y + m_size.height - 0.5f };
        brush->SetColor(Color::d2d1(DINOCOLOR_LIGHTGRAY));
        renderTarget->DrawLine(DPIHandler::adjusted(p1), DPIHandler::adjusted(p2), brush, m_lineWidth);
        curWidth += m_colWidth + m_lineWidth;
    }
}
