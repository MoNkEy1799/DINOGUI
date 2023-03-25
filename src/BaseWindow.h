#pragma once

#include "TemplateBaseClass.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include <string>
#include <vector>
#include <iostream>

namespace DINOGUI
{

class Widget;

class Base : public TemplateWindow<Base>
{
public:
	Base(const std::string& windowName = "DINOGUI", int width = 200, int height = 200, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
	int run();

	LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam);

	void addChild(DINOGUI::Widget* child) { m_childWidgets.push_back(child); };
	int DEBUGchildCount() { return (int)m_childWidgets.size(); };

private:
	IDWriteFactory* m_writeFactory;
	IDWriteTextFormat* m_textFormat;
	ID2D1Factory* m_factory;
	ID2D1HwndRenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_colorBrush;
	D2D1_POINT_2F m_mousePosition;

	std::wstring testText;
	uint32_t textLength;

	std::vector<DINOGUI::Widget*> m_childWidgets;
	std::string m_windowName;
	int m_width, m_height, m_xPos, m_yPos;

	int createFactoryAndDPI();
	void destroyWindow();
	void resizeWindow();
	void paintChildren();
	void mouseMove(int posX, int posY, DWORD flags);
	void leftClick(int posX, int posY, DWORD flags);

	D2D1_SIZE_U getCurrentWindowSize();

	HRESULT	createGraphicsResource();
	void destroyGraphicsResources();
};

}
