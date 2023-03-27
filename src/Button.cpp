#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

DINOGUI::Button::Button(DINOGUI::Base* base, std::string text)
    : m_text(toWideString(text))
{
    m_base = base;
}

DINOGUI::Button::~Button()
{
    safeReleaseInterface(&m_fontFormat);
}

void DINOGUI::Button::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    D2D1_COLOR_F background = m_style.background;
    D2D1_COLOR_F border = m_style.border;
    D2D1_COLOR_F text = m_style.text;
    D2D1_RECT_F rectangle = rect();

    if (m_state == WidgetState::HOVER)
    {
        background = m_style.background_Hover;
        border = m_style.border_Hover;
        text = m_style.text_Hover;
    }

    brush->SetColor(background);
    renderTarget->FillRectangle(rectangle, brush);
    brush->SetColor(border);
    renderTarget->DrawRectangle(rectangle, brush);

    if (!m_fontFormat)
    {
        if (!createFontFormat())
        {
            throw std::runtime_error("Could not create Font Format");
        }
    }

    brush->SetColor(text);
    renderTarget->DrawText(m_text.c_str(), m_text.size(), m_fontFormat, rectangle, brush);
}

void DINOGUI::Button::grid(int row, int col, int rowSpan, int colSpan)
{
    m_base->addWidget(this);
}

void DINOGUI::Button::place(int x, int y)
{
    m_base->addWidget(this);
    m_point = D2D1::Point2F(DPIConverter::PixelsToDips(x), DPIConverter::PixelsToDips(y));
}

void DINOGUI::Button::clicked()
{
    if (m_click)
    {
        m_click();
    }
}

void DINOGUI::Button::connect(void(*func)())
{
    m_click = func;
}

bool DINOGUI::Button::createFontFormat()
{
    HRESULT hResult = m_base->getWriteFactory()->CreateTextFormat(
        toWideString(m_style.fontFamily).c_str(), NULL, m_style.fontWeight,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, m_style.fontSize, L"en-us", &m_fontFormat);

    if (FAILED(hResult))
    {
        return false;
    }

    if (!SUCCEEDED(m_fontFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER)))
    {
        return false;
    }

    if (!SUCCEEDED(m_fontFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER)))
    {
        return false;
    }

    return true;
}
