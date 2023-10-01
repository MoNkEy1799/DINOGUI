#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Checkbox::Checkbox(Core* core, const std::string& text)
    : Widget(core), m_boxPoint(m_point), m_textPoint(m_point),
      m_boxSize({ 12.0f, 12.0f }), m_textSize(m_size), m_text(nullptr)
{
    m_text = new Text(core, text);
    m_text->setAlignment(Alignment::LEFT);
    m_type = WidgetType::CHECKBOX;
    ColorTheme::createDefault(m_theme, m_type);
    m_hoverable = true;
    m_checkable = true;
    m_size = { 80.0f, 20.0f };
    m_minSize = { 16.0f, 16.0f };
}

Checkbox::~Checkbox()
{
    delete m_text;
}

void Checkbox::resize(int width, int height)
{
    m_size = { (float)width, (float)height };
    calculateBoxAndTextLayout();
}

void Checkbox::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_RECT_F rect = DPIHandler::adjusted(currentRect());
    D2D1_RECT_F textRect = DPIHandler::adjusted(currentTextRect());
    D2D1_RECT_F boxRect = DPIHandler::adjusted(currentBoxRect());
    basicDrawBackgroundBorder(rect, renderTarget, brush);
    brush->SetColor(Color::d2d1(m_theme->background2[(int)m_state]));
    renderTarget->FillRectangle(boxRect, brush);
    brush->SetColor(Color::d2d1(m_theme->border2[(int)m_state]));
    renderTarget->DrawRectangle(boxRect, brush, m_theme->width2);
    brush->SetColor(Color::d2d1(m_theme->text[(int)m_state]));
    m_text->draw(textRect, renderTarget, brush);

    if (m_checked)
    {
        renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        brush->SetColor(Color::d2d1(m_theme->text2[(int)m_state]));
        std::array<D2D1_POINT_2F, 3> box = currentCheckbox();
        renderTarget->DrawLine(box[0], box[1], brush, m_theme->width3);
        renderTarget->DrawLine(box[1], box[2], brush, m_theme->width3);
        renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    }
}

void Checkbox::place(int x, int y)
{
    basicPlace(x, y);
    calculateBoxAndTextLayout();
}

Text* Checkbox::getTextWidget()
{
    return m_text;
}

bool Checkbox::isChecked() const
{
    return m_checked;
}

std::array<D2D1_POINT_2F, 3> Checkbox::currentCheckbox() const
{
    D2D1_RECT_F box = currentBoxRect();
    return { {{ box.left + 10.5f, box.top + 3.5f }, { box.left + 5.0f, box.top + 10.0f }, { box.left + 2.0f, box.top + 7.0f }} };
}

void Checkbox::calculateBoxAndTextLayout()
{
    D2D1_RECT_F current = currentRect();
    float pad = m_boxSize.width / 4.0f;
    float midHeight = m_size.height / 2.0f;
    m_boxPoint = { m_point.x + pad, m_point.y + midHeight - m_boxSize.height / 2.0f };
    m_textPoint = { m_point.x + 2 * pad + m_boxSize.width, m_point.y };
    m_textSize = { m_size.width - 2 * pad + m_boxSize.width, m_size.height };
}

D2D1_RECT_F Checkbox::currentTextRect() const
{
    return { m_textPoint.x, m_textPoint.y, m_textPoint.x + m_textSize.width, m_textPoint.y + m_textSize.height };
}

D2D1_RECT_F Checkbox::currentBoxRect() const
{
    return { m_boxPoint.x, m_boxPoint.y, m_boxPoint.x + m_boxSize.width, m_boxPoint.y + m_boxSize.height };
}

