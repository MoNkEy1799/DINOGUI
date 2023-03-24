#pragma once

#include "Widget.h"
#include "Utils.h"

#include <d2d1.h>

namespace DINOGUI
{

class Button : public Widget
{
public:
	Button(DINOGUI::Base& base);

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;

private:
	DINOGUI::Position m_position;
	DINOGUI::Size m_size;
	DINOGUI::Color m_color;
};

}
