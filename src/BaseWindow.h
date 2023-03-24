#pragma once

#include "TemplateBaseClass.h"

#include <Windows.h>
#include <d2d1.h>

#include <string>
#include <vector>
#include <iostream>

namespace DINOGUI
{

class Widget;

class Base : public TemplateWindow<Base>
{
public:
	Base(const std::string& windowName = "DINOGUI");
	int run();

	LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam);

	void addChild(DINOGUI::Widget* child) { m_childWidgets.push_back(child); };

private:
	ID2D1Factory* m_factory;
	ID2D1HwndRenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_colorBrush;
	std::string m_windowName;
	PCWSTR m_pcwName;

	std::vector<DINOGUI::Widget*> m_childWidgets;

	void paintChildren();

	PCWSTR getWindowName();

	HRESULT	createGraphicsResource();
	void destroyGraphicsResources();
};

}
