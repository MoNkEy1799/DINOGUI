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

#define DEBUG_PRINT(x) std::cout << x << std::endl
#define DEBUG_PRINTW(x) std::wcout << x << std::endl

namespace DINOGUI
{

class Widget;
class Button;
class Label;
class Checkbox;
class Textedit;
class Image;
enum class WidgetState { NORMAL, HOVER, CLICKED };
enum class WidgetType { NONE, BUTTON, LABEL, TEXTEDIT, CHECKBOX, IMAGE };

class Core : public TemplateWindow<Core>
{
public:
	void DEBUG_DRAW_RECT(D2D1_RECT_F r);

	Core(const std::string& windowName = "DINOGUI", int width = 200, int height = 200, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
	int run();

	LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam);

	void addWidget(Widget* widget);
	void removeWidget(Widget* widget);
	void addDisplayWidget(Widget* widget);
	void removeDisplayWidget(Widget* widget);
	void redrawScreen() const { InvalidateRect(m_windowHandle, nullptr, false); };

	ID2D1Factory* getFactory() const { return m_factory; };
	IDWriteFactory* getWriteFactory() const { return m_writeFactory; };

	void setHoverWidget(Widget* widget) { m_hoverWidget = widget; };
	void setClickWidget(Widget* widget) { m_clickWidget = widget; };
	void setSelectedWidget(Widget* widget) { m_selectedWidget = widget; };

private:
	IDWriteTextLayout* m_layout;
	IDWriteFactory* m_writeFactory;
	ID2D1Factory* m_factory;
	ID2D1HwndRenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_colorBrush;
	ID2D1StrokeStyle* m_strokeStyle;
	D2D1_POINT_2F m_mousePosition;

	std::vector<Widget*> m_widgets;
	std::vector<Widget*> m_displayWidgets;
	Widget* m_hoverWidget;
	Widget* m_clickWidget;
	Widget* m_selectedWidget;
	std::string m_windowName;
	int m_width, m_height, m_xPos, m_yPos;
	bool m_changeCursor;
	int DEBUG_DrawCalls = 0;

	int createFactoryAndDPI();
	void destroyWindow();
	void resizeWindow();
	void paintWidgets();
	void setCursor();
	void mouseMove(int posX, int posY, DWORD flags);
	void leftClick(int posX, int posY, DWORD flags);
	void leftRelease(int posX, int posY, DWORD flags);
	void processKeys(char key);

	D2D1_SIZE_U getCurrentWindowSize () const;
	Widget* getWidgetUnderMouse(float x, float y) const;

	HRESULT	createGraphicsResource();
	void destroyGraphicsResources();
};

class Widget
{
public:
	static void DEBUG_PRINT_COORDS(D2D1_RECT_F rect, const std::string& str = "");

	Widget(Core* core);
	virtual ~Widget();
	Widget(const Widget&) = delete;
	Widget(Widget&&) = delete;
	Widget& operator=(const Widget&) = delete;
	Widget& operator=(Widget&&) = delete;

	virtual void draw(ID2D1HwndRenderTarget* renderTarget,
					  ID2D1SolidColorBrush* brush,
					  ID2D1StrokeStyle* strokeStyle) = 0;
	virtual void place(int x, int y) = 0;
	virtual void clicked() = 0;
	
	void show();
	void hide();
	void drawBorder(bool draw = true);
	void drawBackground(bool draw = true);
	void setTheme(const ColorTheme& theme);
	void setText(const std::string& text);
	void setFont(const Font& font);
	virtual void setSize(int width, int height);

	WidgetType getWidgetType() const;
	bool contains(float x, float y) const;

	void enterEvent();
	void leaveEvent();
	void clickEvent();
	void releaseEvent();
	void unselectEvent();
	static D2D1_POINT_2F drawingAdjusted(D2D1_POINT_2F point);
	static D2D1_RECT_F drawingAdjusted(D2D1_RECT_F rect);
	static bool hoverableWidget(const WidgetType& type);
	static bool clickableWidget(const WidgetType& type);
	static bool selectableWidget(const WidgetType& type);

protected:
	IDWriteTextFormat* m_fontFormat;
	D2D1_POINT_2F m_point;
	D2D1_SIZE_F m_size;
	Core* m_core;
	ColorTheme m_theme;
	Font m_font;
	WidgetState m_state;
	WidgetType m_type;
	std::string m_text;
	bool m_drawBackground, m_drawBorder;
	bool m_hover;

	D2D1_RECT_F currentRect() const;
	bool createFontFormat();
};

class Button : public Widget
{
public:
	Button(Core* core, const std::string& text = "", std::function<void()> function = nullptr);
	~Button() = default;
	Button(const Button&) = delete;
	Button(Button&&) = delete;
	Button& operator=(const Button&) = delete;
	Button& operator=(Button&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget,
			  ID2D1SolidColorBrush* brush,
			  ID2D1StrokeStyle* strokeStyle) override;
	void place(int x, int y) override;
	void clicked() override;

	void connect(std::function<void()> function);

private:
	std::function<void()> m_clickFunction;
};

class Label : public Widget
{
public:
	Label(Core* core, const std::string& text = "");
	~Label() = default;
	Label(const Label&) = delete;
	Label(Label&&) = delete;
	Label& operator=(const Label&) = delete;
	Label& operator=(Label&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget,
		      ID2D1SolidColorBrush* brush,
		      ID2D1StrokeStyle* strokeStyle) override;
	void place(int x, int y) override;
	void clicked() override {};
};

class Checkbox : public Widget
{
public:
	Checkbox(Core* core, const std::string& text = "");
	~Checkbox();
	Checkbox(const Checkbox&) = delete;
	Checkbox(Checkbox&&) = delete;
	Checkbox& operator=(const Checkbox&) = delete;
	Checkbox& operator=(Checkbox&&) = delete;

	void setSize(int width, int height) override;
	void draw(ID2D1HwndRenderTarget* renderTarget,
			  ID2D1SolidColorBrush* brush,
			  ID2D1StrokeStyle* strokeStyle) override;
	void place(int x, int y) override;
	void clicked() override;

private:
	ID2D1PathGeometry* m_checkmark;
	D2D1_POINT_2F m_boxPoint, m_textPoint;
	D2D1_SIZE_F m_boxSize, m_textSize;
	bool m_check;

	bool createPathGeometry();
	void calculateBoxAndTextLayout();
	D2D1_RECT_F currentTextRect() const;
	D2D1_RECT_F currentBoxRect() const;
};

class Textedit : public Widget
{
public:
	Textedit(Core* core);
	~Textedit() = default;
	Textedit(const Textedit&) = delete;
	Textedit(Textedit&&) = delete;
	Textedit& operator=(const Textedit&) = delete;
	Textedit& operator=(Textedit&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget,
		ID2D1SolidColorBrush* brush,
		ID2D1StrokeStyle* strokeStyle) override;
	void place(int x, int y) override;
	void clicked() override;

	void keyInput(char key);

private:
	D2D1_POINT_2F m_cursorPoint;
	D2D1_SIZE_F m_cursorSize;
	bool m_selected;
	bool m_drawCursor;

	D2D1_RECT_F currentCursorLine() const;
	D2D1_RECT_F currentTextRect() const;

	static void switchCursor(HWND, uint32_t, uint64_t classPtr, DWORD);
};

}
