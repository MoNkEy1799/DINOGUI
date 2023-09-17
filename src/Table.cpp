#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

using namespace DINOGUI;

Table::Table(Core* core)
    : Widget(core), m_rows(1), m_cols(1), m_entries({ {nullptr, 1, 1} }),
      m_lineWidth(1.0f), m_rowHeight(0.0f), m_colWidth(0.0f)
{
    m_type = WidgetType::TABLE;
    ColorTheme::createDefault(m_theme, m_type);
    m_drawBackground = true;
    m_drawBorder = true;
    m_size = { 120.0f, 140.0f };
    m_theme->background = { Color{ 255, 255, 255 }, Color{ 255, 255, 255 }, Color{ 255, 255, 255 } };
}

Table::~Table()
{
    for (GridEntry<Text*> entry: m_entries)
    {
        delete entry.entry;
    }
}

void Table::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_RECT_F rect = DPIHandler::adjusted(currentRect());
    basicDrawBackgroundBorder(rect, renderTarget, brush);

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
    if (row + rowSpan > m_rows)
    {
        for (int i = m_rows - 1; i < row + rowSpan; i++)
        {
            for (int j = 0; j < m_cols; j++)
            {
                m_entries.push_back({ nullptr, 1, 1 });
            }
        }
        m_rows = row + rowSpan;
    }
    if (col + colSpan > m_cols)
    {
        for (int j = m_cols; j < col + colSpan; j++)
        {
            for (int i = 0; i < m_rows; i++)
            {
                m_entries.insert(m_entries.begin() + i * j + j + i, { nullptr, 1, 1 });
            }
        }
        m_cols = col + colSpan;
    }
    GridEntry<Text*>& entry = m_entries[(uint32_t)row * m_cols + col];
    if (text != "")
    {
        entry.entry = new Text(m_core, text);
    }
    entry.rowSpan = (rowSpan < 1) ? 1 : rowSpan;
    entry.colSpan = (colSpan < 1) ? 1 : colSpan;
}

void Table::setLineWidth(float lineWidth)
{
    m_lineWidth = lineWidth;
}

void Table::drawTextInCell(int row, int col, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    GridEntry<Text*>& entry = m_entries[row * m_cols + col];
    D2D1_RECT_F cell = DPIHandler::adjusted({ m_point.x + (col + 1) * m_lineWidth + col * m_colWidth,
                         m_point.y + (row + 1) * m_lineWidth + row * m_rowHeight,
                         m_point.x + (col + 1 + entry.colSpan) * m_lineWidth + (col + entry.colSpan) * m_colWidth,
                         m_point.y + (row + 1 + entry.rowSpan) * m_lineWidth + (row + entry.rowSpan) * m_rowHeight });
    if (entry.rowSpan > 1 || entry.colSpan > 1)
    {
        brush->SetColor(Color::d2d1(m_theme->background[(int)m_state]));
        renderTarget->FillRectangle(cell, brush);
    }
    if (entry.entry)
    {
        entry.entry->setColor(m_theme->text[(int)m_state]);
        entry.entry->draw(cell, renderTarget, brush);
    }
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
        brush->SetColor(Color::d2d1(m_theme->border2[(int)m_state]));
        renderTarget->DrawLine(DPIHandler::adjusted(p1), DPIHandler::adjusted(p2), brush, m_lineWidth);
        curHeight += m_rowHeight + m_lineWidth;
    }
    for (int col = 0; col < m_cols - 1; col++)
    {
        p1 = { curWidth, m_point.y + 1.0f };
        p2 = { curWidth, m_point.y + m_size.height - 0.5f };
        brush->SetColor(Color::d2d1(m_theme->border2[(int)m_state]));
        renderTarget->DrawLine(DPIHandler::adjusted(p1), DPIHandler::adjusted(p2), brush, m_lineWidth);
        curWidth += m_colWidth + m_lineWidth;
    }
}
