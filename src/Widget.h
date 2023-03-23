#pragma once

#include "BaseWindow.h"

#include <d2d1.h>

namespace DINOGUI
{

class Widget
{
public:
	Widget(DINOGUI::Base& base) : m_base(base) { base.addChild(this); };
	
	virtual void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) = 0;

protected:
	DINOGUI::Base& m_base;
};

}
