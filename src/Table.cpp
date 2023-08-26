#include "Dinogui.h"
#include "Utils.h"

using namespace DINOGUI;

Table::Table(Core* core)
	: Widget(core), m_rows(0), m_cols(0), m_lineWidth(1.0f)
{
    m_type = WidgetType::TABLE;
    m_drawBackground = true;
    m_drawBorder = true;
    m_size = { 120.0f, 140.0f };
    m_rows = 10;
    m_cols = 4;
}

Table::~Table()
{
}

void Table::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    m_theme.bg = DINOCOLOR_LIGHTGRAY;
    drawBasicShape(renderTarget, brush);
    D2D1_COLOR_F colText = Color::d2d1(m_theme.txt);

    if (!m_fontFormat)
    {
        throwIfFailed(createFontFormat(), "Failed to create text format");
    }

    float colWidth = (m_size.width - (m_cols + 1) * m_lineWidth) / m_cols;
    float rowHeight = (m_size.height - (m_rows + 1) * m_lineWidth) / m_rows;
    D2D1_RECT_F cell;
    for (int row = 0; row < m_rows; row++)
    {
        for (int col = 0; col < m_cols; col++)
        {
            cell = { m_point.x + (col + 1) * m_lineWidth + col * colWidth,
                     m_point.y + (row + 1) * m_lineWidth + row * rowHeight,
                     m_point.x + (col + 1) * m_lineWidth + (col + 1) * colWidth,
                     m_point.y + (row + 1) * m_lineWidth + (row + 1) * rowHeight };
            brush->SetColor({ 255, 255, 255, 255 });
            renderTarget->FillRectangle(DPIHandler::adjusted(cell), brush);
        }
    }
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
    adjustSize();
}

void Table::adjustSize()
{
    float colWidth = std::floor((m_size.width - (m_cols + 1) * m_lineWidth) / m_cols);
    float restWidth = m_size.width - (m_cols + 1) * m_lineWidth - m_cols * colWidth;
    float rowHeight = std::floor((m_size.height - (m_rows + 1) * m_lineWidth) / m_rows);
    float restHeight = m_size.height - (m_rows + 1) * m_lineWidth - m_rows * rowHeight;
    m_size.width -= std::floor(restWidth);
    m_size.height -= std::floor(restHeight);
}
