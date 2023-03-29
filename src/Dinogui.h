#pragma once
#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(linker, "/ENTRY:mainCRTStartup")

#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>

namespace DINOGUI
{

class Widget;
class Button;
class Label;
class Checkbox;
class LineEdit;
class Image;
enum class WidgetState { NORMAL, HOVER, CLICKED, SELECTED };
enum class WidgetType { NONE, BUTTON, LABEL, LINEEDIT, CHECKBOX, IMAGE };

class Base : public TemplateWindow<Base>
{
public:
	Base(const std::string& windowName = "DINOGUI", int width = 200, int height = 200, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
	int run();

	LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam);

	void addWidget(Widget* widget) { m_widgets.push_back(widget); };
	void removeWidget(Widget* widget) { m_widgets.erase(std::remove(m_widgets.begin(), m_widgets.end(), widget), m_widgets.end()); };
	void addDisplayWidget(Widget* widget) { m_displayWidgets.push_back(widget); };
	void removeDisplayWidget(Widget* widget) { m_displayWidgets.erase(std::remove(m_displayWidgets.begin(), m_displayWidgets.end(), widget), m_displayWidgets.end()); };
	void redrawScreen() { InvalidateRect(m_windowHandle, nullptr, false); };

	IDWriteFactory* getWriteFactory() { return m_writeFactory; };

private:
	IDWriteFactory* m_writeFactory;
	ID2D1Factory* m_factory;
	ID2D1HwndRenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_colorBrush;
	D2D1_POINT_2F m_mousePosition;

	std::vector<Widget*> m_widgets;
	std::vector<Widget*> m_displayWidgets;
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
	void leftRelease(int posX, int posY, DWORD flags);

	D2D1_SIZE_U getCurrentWindowSize();
	Widget* getWidgetUnderMouse(int x, int y);
	bool hoverableWidget(Widget* widget);
	bool clickableWidget(Widget* widget);
	bool selectableWidget(Widget* widget);

	HRESULT	createGraphicsResource();
	void destroyGraphicsResources();
};

class Widget
{
public:
	Widget();
	~Widget();
	Widget(const Widget&) = delete;
	Widget(Widget&&) = delete;
	Widget& operator=(const Widget&) = delete;
	Widget& operator=(Widget&&) = delete;

	virtual void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) = 0;
	virtual void grid(int row, int col, int rowSpan, int colSpan) = 0;
	virtual void place(int x, int y) = 0;
	
	void setTheme(const ColorTheme& theme);
	void setFont(const Font& font);
	void setWidgetState(const WidgetState& state, bool redraw = true);
	WidgetType getWidgetType();
	bool contains(int x, int y);
	bool createFontFormat();
	void show();
	void hide();

	void DEBUGSIZE() { std::cout << "SIZE: " << m_size.width << " | " << m_size.height << std::endl; };
	void DEBUGPOS() { std::cout << "POS: " << m_point.x << " | " << m_point.y << std::endl; };

protected:
	IDWriteTextFormat* m_fontFormat;
	D2D1_POINT_2F m_point;
	D2D1_SIZE_F m_size;
	Base* m_base;
	ColorTheme m_theme;
	Font m_font;
	WidgetState m_state;
	WidgetType m_type;

	D2D1_RECT_F currentRect();
};

class Button : public Widget
{
public:
	Button(Base* base, const std::string& text = "", std::function<void()> function = nullptr);
	~Button() = default;
	Button(const Button&) = delete;
	Button(Button&&) = delete;
	Button& operator=(const Button&) = delete;
	Button& operator=(Button&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void grid(int row, int col, int rowSpan, int colSpan) override;
	void place(int x, int y) override;

	void clicked();
	void connect(std::function<void()> function);

private:
	std::string m_text;
	std::function<void()> m_click;
};

class Label : public Widget
{
public:
	Label(Base* base, const std::string& text = "");
	~Label() = default;
	Label(const Label&) = delete;
	Label(Label&&) = delete;
	Label& operator=(const Label&) = delete;
	Label& operator=(Label&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void grid(int row, int col, int rowSpan, int colSpan) override;
	void place(int x, int y) override;

private:
	std::string m_text;
};

class Checkbox : public Widget
{
public:
	Checkbox(Base* base, const std::string& text = "");
	~Checkbox() = default;
	Checkbox(const Checkbox&) = delete;
	Checkbox(Checkbox&&) = delete;
	Checkbox& operator=(const Checkbox&) = delete;
	Checkbox& operator=(Checkbox&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void grid(int row, int col, int rowSpan, int colSpan) override;
	void place(int x, int y) override;

	void check();

private:
	std::string m_text;
	D2D1_RECT_F m_box;
	bool m_check;
};

}
