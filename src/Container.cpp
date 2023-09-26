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
}

Container::~Container()
{
    for (Container* container : m_containers)
    {
        delete container;
    }
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
        auto comp = [](const GridEntry<LayoutObject*>& e1, const GridEntry<LayoutObject*>& e2)
        {
            return true;
        };
        auto max = std::max_element(m_objects.begin(), m_objects.end(), comp);
    }
}