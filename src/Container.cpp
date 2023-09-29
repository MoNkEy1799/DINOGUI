#include "Dinogui.h"
#include "Utils.h"

#include <cmath>
#include <vector>
#include <array>
#include <string>

using namespace DINOGUI;

Container::Container(Core* core)
	: LayoutObject(LayoutObjectType::CONTAINER), m_margins({{ 6.0f, 6.0f, 6.0f, 6.0f }}),
      m_spacing({{ 6.0f, 6.0f }}), m_objects(), m_rows(0), m_cols(0), m_core(core)
{
    CoreInterface::addContainer(m_core, this);
}

Container::~Container()
{
    CoreInterface::removeContainer(m_core, this);
}

void Container::addWidget(Widget* widget, int row, int col, int rowSpan, int colSpan)
{
    if (row + rowSpan > m_rows)
    {
        for (int i = m_rows; i < row + rowSpan; i++)
        {
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
            for (int i = 0; i < m_rows; i++)
            {
                m_objects.insert(m_objects.begin() + i * j + j + i, { nullptr, 1, 1 });
            }
        }
        m_cols = col + colSpan;
    }
    GridEntry<LayoutObject*>& entry = m_objects[(size_t)row * m_cols + col];
    entry.entry = widget;
    entry.rowSpan = (rowSpan < 1) ? 1 : rowSpan;
    entry.colSpan = (colSpan < 1) ? 1 : colSpan;
    updatePositionAndSizes();
}

void Container::addContainer(Container* container, int row, int col, int rowSpan, int colSpan)
{
    if (row + rowSpan > m_rows)
    {
        for (int i = m_rows; i < row + rowSpan; i++)
        {
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
            for (int i = 0; i < m_rows; i++)
            {
                m_objects.insert(m_objects.begin() + i * j + j + i, { nullptr, 1, 1 });
            }
        }
        m_cols = col + colSpan;
    }
    GridEntry<LayoutObject*>& entry = m_objects[(size_t)row * m_cols + col];
    entry.entry = container;
    entry.rowSpan = (rowSpan < 1) ? 1 : rowSpan;
    entry.colSpan = (colSpan < 1) ? 1 : colSpan;
    updatePositionAndSizes();
}

void Container::updatePositionAndSizes()
{
    for (int row = 0; row < m_rows; row++)
    {
        auto max = std::max_element(m_objects.begin(), m_objects.end(), compareWidth);
    }
}

bool Container::compareWidth(const GridEntry<LayoutObject*>& e1, const GridEntry<LayoutObject*>& e2)
{
    int width1, width2;
    LayoutObject* o1 = e1.entry;
    LayoutObject* o2 = e2.entry;
    if (e1.entry->layoutType == LayoutObjectType::WIDGET)
    {
        width1 = ((Widget*)o1)->getResizeState().minSize.width;
    }
    else
    {
        width1 = ((Container*)o1)->m_size.width;
    }
    if (e2.entry->layoutType == LayoutObjectType::WIDGET)
    {
        width2 = ((Widget*)o1)->getResizeState().minSize.width;
    }
    else
    {
        width2 = ((Container*)o1)->m_size.width;
    }
    return width1 < width2;
}

bool Container::compareHeight(const GridEntry<LayoutObject*>& e1, const GridEntry<LayoutObject*>& e2)
{
    int height1, height2;
    LayoutObject* o1 = e1.entry;
    LayoutObject* o2 = e2.entry;
    if (e1.entry->layoutType == LayoutObjectType::WIDGET)
    {
        height1 = ((Widget*)o1)->getResizeState().minSize.width;
    }
    else
    {
        height1 = ((Container*)o1)->m_size.width;
    }
    if (e2.entry->layoutType == LayoutObjectType::WIDGET)
    {
        height2 = ((Widget*)o1)->getResizeState().minSize.width;
    }
    else
    {
        height2 = ((Container*)o1)->m_size.width;
    }
    return height1 < height2;
}
