#pragma once

#include "Widget.h"
#include "Utils.h"

#include <d2d1.h>
#include <string>

namespace DINOGUI
{

class Base;

class Button : public Widget
{
public:
	Button(DINOGUI::Base* base, std::string text = "", DINOGUI::Font font = {10.0f, "Sans-Serif", 400});
	~Button();

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int row, int col, int rowSpan, int colSpan) override;
	void hover(int x, int y);

private:
	D2D1_RECT_F m_rect;
	D2D1_COLOR_F m_color;
	IDWriteTextFormat* m_fontFormat;
	DINOGUI::Font m_font;

	DINOGUI::Base* m_base;
	std::wstring m_text;

	bool insideRect(int x, int y);

	void createFontFormat();
};

}
