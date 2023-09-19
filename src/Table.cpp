#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <vector>
#include <array>
#include <numeric>
#include <algorithm>

using namespace DINOGUI;

Table::Table(Core* core)
    : Widget(core), m_rows(1), m_cols(1), m_entries({ {nullptr, 1, 1} }),
      m_lineWidth(1.0f), m_rowHeights({ 0.0f }), m_colWidths({ 0.0f }),
      m_rowWeights({ 1.0f }), m_colWeights({ 1.0f }), m_prevBorderWidth(1.0f)
{
    m_type = WidgetType::TABLE;
    ColorTheme::createDefault(m_theme, m_type);
    m_drawBackground = true;
    m_drawBorder = true;
    m_size = { 120.0f, 140.0f };
    m_maxSize = { 1e6f, 1e6f };
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
    std::cout << m_rowWeights.size() << m_colWeights.size() << std::endl;
    D2D1_RECT_F rect = DPIHandler::adjusted(currentRect());
    basicDrawBackgroundBorder(rect, renderTarget, brush);

    if (m_prevBorderWidth != m_theme->width)
    {
        calculateDimensions();
    }

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
        for (int i = m_rows; i < row + rowSpan; i++)
        {
            m_rowWeights.push_back(1.0f);
            m_rowHeights.push_back(0.0f);
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
            m_colWeights.push_back(1.0f);
            m_colWidths.push_back(0.0f);
            for (int i = 0; i < m_rows; i++)
            {
                m_entries.insert(m_entries.begin() + i * j + j + i, { nullptr, 1, 1 });
            }
        }
        m_cols = col + colSpan;
    }
    GridEntry<Text*>& entry = m_entries[(size_t)row * m_cols + col];
    if (text != "")
    {
        entry.entry = new Text(m_core, text);
    }
    entry.rowSpan = (rowSpan < 1) ? 1 : rowSpan;
    entry.colSpan = (colSpan < 1) ? 1 : colSpan;
    calculateDimensions();
}

std::vector<Text*> Table::getTextWidgets()
{
    std::vector<Text*> vec;
    for (GridEntry<Text*> entry : m_entries)
    {
        vec.push_back(entry.entry);
    }
    return vec;
}

Text* Table::getTextWidget(int row, int col)
{
    return m_entries[(size_t)row * m_rows + col].entry;
}

void Table::setRowWeight(int row, float weight)
{
    if (row < 0 || row >= m_rows)
    {
        return;
    }
    m_rowWeights[row] = weight;
    calculateDimensions();
}

void Table::setColWeight(int col, float weight)
{
    if (col < 0 || col >= m_cols)
    {
        return;
    }
    m_colWeights[col] = weight;
    calculateDimensions();
}

void Table::setLineWidth(float lineWidth)
{
    m_lineWidth = lineWidth;
    calculateDimensions();
}

void Table::drawTextInCell(int row, int col, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    GridEntry<Text*>& entry = m_entries[row * m_cols + col];
    float left = std::accumulate(m_colWidths.begin(), m_colWidths.begin() + col, 0.0f)
        + m_theme->width + col * m_lineWidth;
    float top = std::accumulate(m_rowHeights.begin(), m_rowHeights.begin() + row, 0.0f)
        + m_theme->width + row * m_lineWidth;
    float right = std::accumulate(m_colWidths.begin(), m_colWidths.begin() + col + entry.colSpan, 0.0f)
        + m_theme->width + (col + entry.colSpan - 1) * m_lineWidth;
    float bottom = std::accumulate(m_rowHeights.begin(), m_rowHeights.begin() + row + entry.rowSpan, 0.0f)
        + m_theme->width + (row + entry.rowSpan - 1) * m_lineWidth;
    D2D1_RECT_F cell = DPIHandler::adjusted({ m_point.x + left, m_point.y + top,
        m_point.x + right, m_point.y + bottom });
    if (entry.rowSpan > 1 || entry.colSpan > 1)
    {
        brush->SetColor(Color::d2d1(m_theme->background[(int)m_state]));
        //brush->SetColor(Color::d2d1(DINOCOLOR_RED));
        renderTarget->FillRectangle(cell, brush);
    }
    if (entry.entry)
    {
        brush->SetColor(Color::d2d1(m_theme->text[(int)m_state]));
        entry.entry->draw(cell, renderTarget, brush);
    }
}

void Table::drawCellLines(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_POINT_2F p1, p2;
    float curHeight = m_point.y + m_rowHeights[0] + m_theme->width / 2.0f + m_lineWidth / 2.0f;
    float curWidth = m_point.x + m_colWidths[0] + m_theme->width / 2.0f + m_lineWidth / 2.0f;
    for (int row = 0; row < m_rows - 1; row++)
    {
        p1 = { m_point.x + m_theme->width, curHeight };
        p2 = { m_point.x + m_size.width - (m_theme->width - 0.5f), curHeight };
        brush->SetColor(Color::d2d1(m_theme->border2[(int)m_state]));
        renderTarget->DrawLine(DPIHandler::adjusted(p1), DPIHandler::adjusted(p2), brush, m_lineWidth);
        curHeight += m_rowHeights[row + 1] + m_lineWidth;
    }
    for (int col = 0; col < m_cols - 1; col++)
    {
        p1 = { curWidth, m_point.y + m_theme->width };
        p2 = { curWidth, m_point.y + m_size.height - (m_theme->width - 0.5f) };
        brush->SetColor(Color::d2d1(m_theme->border2[(int)m_state]));
        renderTarget->DrawLine(DPIHandler::adjusted(p1), DPIHandler::adjusted(p2), brush, m_lineWidth);
        curWidth += m_colWidths[col + 1] + m_lineWidth;
    }
}

void Table::calculateDimensions()
{
    float partWidth = (m_size.width - m_theme->width - (m_cols - 1) * m_lineWidth)
        / std::accumulate(m_colWeights.begin(), m_colWeights.end(), 0.0f);
    float partHeight = (m_size.height - m_theme->width - (m_rows - 1) * m_lineWidth)
        / std::accumulate(m_rowWeights.begin(), m_rowWeights.end(), 0.0f);
    std::transform(m_colWeights.begin(), m_colWeights.end(), m_colWidths.begin(),
        [partWidth](float weight) { return partWidth * weight; });
    std::transform(m_rowWeights.begin(), m_rowWeights.end(), m_rowHeights.begin(),
        [partHeight](float weight) { return partHeight * weight; });
}
