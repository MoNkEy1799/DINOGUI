#include "Dinogui.h"
#include "Utils.h"

#include <cmath>
#include <vector>
#include <array>
#include <string>

using namespace DINOGUI;

GridManager::GridManager(Core* core)
	: m_margins({{ 6.0f, 6.0f, 6.0f, 6.0f }}), m_spacing({{ 4.0f, 4.0f }}),
      m_widgets(), m_rows(0), m_cols(0), m_core(core)
{
}

void GridManager::addWidget(Widget* widget, int row, int col, int rowSpan, int colSpan)
{
    if (row + rowSpan > m_rows)
    {
        for (int i = m_rows - 1; i < row + rowSpan; i++)
        {
            for (int j = 0; j < m_cols; j++)
            {
                m_widgets.push_back({ nullptr, 1, 1 });
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
                m_widgets.insert(m_widgets.begin() + i * j + j + i, { nullptr, 1, 1 });
            }
        }
        m_cols = col + colSpan;
    }
    GridEntry<Widget*>& entry = m_widgets[(size_t)row * m_cols + col];
    entry.entry = widget;
    entry.rowSpan = (rowSpan < 1) ? 1 : rowSpan;
    entry.colSpan = (colSpan < 1) ? 1 : colSpan;
    updateSizes();
}

void GridManager::updateSizes()
{
    Size<int> windowSize = m_core->getCurrentWindowSize();


}