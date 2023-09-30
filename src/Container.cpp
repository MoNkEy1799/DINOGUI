#include "Dinogui.h"
#include "Utils.h"

#include <cmath>
#include <vector>
#include <array>
#include <string>

using namespace DINOGUI;

Container::Container(Core* core)
	: LayoutObject(LayoutObjectType::CONTAINER), m_margins({{ 6.0f, 6.0f, 6.0f, 6.0f }}),
      m_spacing({{ 6.0f, 6.0f }}), m_objects(), m_rows(0), m_cols(0), m_core(core), m_size({ 0.0f, 0.0f })
{
    CoreInterface::addContainer(m_core, this);
}

Container::~Container()
{
    CoreInterface::removeContainer(m_core, this);
}

void Container::addWidget(Widget* widget, int row, int col, int rowSpan, int colSpan)
{
    Size<float> minSize = widget->resizeState.minSize;
    extendVector(minSize, row, col, rowSpan, colSpan);
    GridEntry<LayoutObject*>& entry = m_objects[(size_t)row * m_cols + col];
    entry.entry = widget;
    entry.rowSpan = std::max(1, rowSpan);
    entry.colSpan = std::max(1, colSpan);
    updatePositionAndSizes();
}

void Container::addContainer(Container* container, int row, int col, int rowSpan, int colSpan)
{
    Size<float> minSize = container->m_size;
    extendVector(minSize, row, col, rowSpan, colSpan);
    GridEntry<LayoutObject*>& entry = m_objects[(size_t)row * m_cols + col];
    entry.entry = container;
    entry.rowSpan = std::max(1, rowSpan);
    entry.colSpan = std::max(1, colSpan);
    updatePositionAndSizes();
}

void Container::extendVector(Size<float> minSize, int row, int col, int rowSpan, int colSpan)
{
    if (row + rowSpan > m_rows)
    {
        for (int i = m_rows; i < row + rowSpan; i++)
        {
            m_rowHeights.push_back(minSize.height);
            for (int j = 0; j < m_cols; j++)
            {
                m_objects.push_back({ nullptr, 1, 1 });
            }
        }
        m_rows = row + rowSpan;
    }
    if (col + colSpan > m_cols)
    {
        for (int j = m_cols; j < col + colSpan; j++)
        {
            m_colWidths.push_back(minSize.width);
            for (int i = 0; i < m_rows; i++)
            {
                m_objects.insert(m_objects.begin() + i * j + j + i, { nullptr, 1, 1 });
            }
        }
        m_cols = col + colSpan;
    }
    for (int i = row; i < row + rowSpan; i++)
    {
        m_rowHeights[i] = std::max(minSize.height, m_rowHeights[i]);
    }
    for (int j = col; j < col + colSpan; j++)
    {
        m_colWidths[j] = std::max(minSize.width, m_colWidths[j]);
    }
}

void Container::updatePositionAndSizes()
{
    Size<int> winSize = m_core->resizeState.maxSize;

}
