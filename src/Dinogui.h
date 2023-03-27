#pragma once
#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(linker, "/ENTRY:mainCRTStartup")

#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <vector>
#include <iostream>

namespace DINOGUI
{

class Widget;
class Button;
class Label;
class LineEdit;
class Checkbox;
class Image;
enum class WidgetState { NORMAL, HOVER, CLICK, SELECT };

class Base : public TemplateWindow<Base>
{
public:
	Base(const std::string& windowName = "DINOGUI", int width = 200, int height = 200, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
	int run();

	LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam);

	void addWidget(DINOGUI::Widget* widget) { m_displayWidgets.push_back(widget); };

	IDWriteFactory* getWriteFactory() { return m_writeFactory; };

private:
	IDWriteFactory* m_writeFactory;
	ID2D1Factory* m_factory;
	ID2D1HwndRenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_colorBrush;
	D2D1_POINT_2F m_mousePosition;

	std::vector<DINOGUI::Widget*> m_displayWidgets;
	Widget* m_hoverWidget;
	Widget* m_clickWidget;
	std::string m_windowName;
	int m_width, m_height, m_xPos, m_yPos;

	int createFactoryAndDPI();
	void destroyWindow();
	void resizeWindow();
	void paintWidgets();
	void mouseMove(int posX, int posY, DWORD flags);
	void leftClick(int posX, int posY, DWORD flags);

	D2D1_SIZE_U getCurrentWindowSize();
	Widget* getWidgetUnderMouse(int x, int y);

	HRESULT	createGraphicsResource();
	void destroyGraphicsResources();
};

class Widget
{
public:
	virtual void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) = 0;
	virtual void grid(int row, int col, int rowSpan, int colSpan) = 0;
	virtual void place(int x, int y) = 0;
	
	void setStyle(const DINOGUI::Style& style) { m_style = style; };
	bool contains(int x, int y) { return ((x > m_point.x && x < m_point.x + m_size.width) && (y > m_point.y && y < m_point.y + m_size.height)); };
	void setWidgetState(const WidgetState& state) { m_state = state; };

	void DEBUGSIZE() { std::cout << "SIZE: " << m_size.width << " | " << m_size.height << std::endl; };
	void DEBUGPOS() { std::cout << "POS: " << m_point.x << " | " << m_point.y << std::endl; };

	static std::wstring toWideString(const std::string& string) { return std::wstring(string.begin(), string.end()); };

protected:
	IDWriteTextFormat* m_fontFormat = nullptr;
	D2D1_POINT_2F m_point = { 0.0f, 0.0f };
	D2D1_SIZE_F m_size = { 60.0f, 20.0f };
	DINOGUI::Base* m_base = nullptr;
	DINOGUI::Style m_style = DINOGUI_STYLE_DARK;
	WidgetState m_state = WidgetState::NORMAL;

	D2D1_RECT_F rect() { return D2D1::RectF(m_point.x, m_point.y, m_point.x + m_size.width, m_point.y + m_size.height); };
};

class Button : public Widget
{
public:
	Button(DINOGUI::Base* base, std::string text = "");
	~Button();

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void grid(int row, int col, int rowSpan, int colSpan) override;
	void place(int x, int y) override;

private:
	std::wstring m_text;

	bool createFontFormat();
};

}
