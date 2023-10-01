#include "Dinogui.h"
#include "Utils.h"

#include <cmath>
#include <vector>
#include <array>
#include <string>
#include <numeric>

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
    Size<float> curSize = widget->resizeState.size;
    extendVector(minSize, curSize, row, col, rowSpan, colSpan);
    GridEntry<LayoutObject*>& entry = m_objects[(size_t)row * m_cols + col];
    entry.entry = widget;
    entry.rowSpan = std::max(1, rowSpan);
    entry.colSpan = std::max(1, colSpan);
    updatePositionAndSizes();
}

void Container::addContainer(Container* container, int row, int col, int rowSpan, int colSpan)
{
    Size<float> size = container->m_size;
    extendVector(size, size, row, col, rowSpan, colSpan);
    GridEntry<LayoutObject*>& entry = m_objects[(size_t)row * m_cols + col];
    entry.entry = container;
    entry.rowSpan = std::max(1, rowSpan);
    entry.colSpan = std::max(1, colSpan);
    updatePositionAndSizes();
}

void Container::extendVector(Size<float> minSize, Size<float> curSize, int row, int col, int rowSpan, int colSpan)
{
    if (row + rowSpan > m_rows)
    {
        for (int i = m_rows; i < row + rowSpan; i++)
        {
            m_minHeights.push_back(minSize.height);
            m_curHeights.push_back(curSize.height);
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
            m_minWidths.push_back(minSize.width);
            m_curWidths.push_back(curSize.width);
            for (int i = 0; i < m_rows; i++)
            {
                m_objects.insert(m_objects.begin() + i * j + j + i, { nullptr, 1, 1 });
            }
        }
        m_cols = col + colSpan;
    }
    for (int i = row; i < row + rowSpan; i++)
    {
        m_minHeights[i] = std::max(minSize.height, m_minHeights[i]);
        m_curHeights[i] = std::max(curSize.height, m_curHeights[i]);
    }
    for (int j = col; j < col + colSpan; j++)
    {
        m_minWidths[j] = std::max(minSize.width, m_minWidths[j]);
        m_curWidths[j] = std::max(curSize.width, m_curWidths[j]);
    }
}

void Container::updatePositionAndSizes()
{
    Size<int> winSize = m_core->resizeState.maxSize;
    float sumWidth = std::accumulate(m_curWidths.begin(), m_curWidths.end(), 0.0f);
    float sumHeight = std::accumulate(m_curHeights.begin(), m_curHeights.end(), 0.0f);
    float shrinkWidth = (sumWidth + m_margins[0] + m_margins[2] + (m_cols - 1) * m_spacing[0]) - winSize.width;
    float shrinkHeight = (sumWidth + m_margins[1] + m_margins[3] + (m_rows - 1) * m_spacing[1]) - winSize.height;
    
    for (int i = 0; i < m_rows; i++)
    {
        for (int j = 0; j < m_cols; j++)
        {
            Size<float> size = getSize(m_objects[i * m_cols + j].entry);
        }
    }
}

Size<float> Container::getSize(LayoutObject* object)
{
    switch (object->layoutType)
    {
    case LayoutObjectType::WIDGET:
        return ((Widget*)object)->resizeState.size;

    case LayoutObjectType::CONTAINER:
        return ((Container*)object)->m_size;
    }
    return { 0.0f, 0.0f };
}
