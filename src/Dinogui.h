#pragma once
#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(linker, "/ENTRY:mainCRTStartup")

#include "Utils.h"

#include <Windows.h>
#include <wincodec.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <utility>

#define DEBUG_PRINT(x, m) std::cout << m << " : " << x << std::endl
#define DEBUG_PRINTW(x) std::wcout << x << std::endl

namespace DINOGUI
{

class Widget;
class Button;
class Label;
class Checkbox;
class Textedit;
class Image;
class Canvas;
enum class WidgetState { NORMAL, HOVER, CLICKED };
enum class WidgetType { NONE, BUTTON, LABEL, CHECKBOX, TEXTEDIT, IMAGE, CANVAS, TABLE, COMBOBOX };

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
	IWICImagingFactory* getImageFactory() const { return m_imageFactory; };

	void setHoverWidget(Widget* widget) { m_hoverWidget = widget; };
	void setClickWidget(Widget* widget) { m_clickWidget = widget; };
	void setSelectedWidget(Widget* widget) { m_selectedWidget = widget; };

private:
	ID2D1Factory* m_factory;
	IDWriteFactory* m_writeFactory;
	IWICImagingFactory* m_imageFactory;
	ID2D1HwndRenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_colorBrush;
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
	void processOtherKeys(uint32_t key);

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

	virtual void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) = 0;
	virtual void place(int x, int y) = 0;
	virtual void clicked(float mouseX, float mouseY) = 0;
	
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

	void receiveEvent(Event* event);
	void enterEvent();
	void leaveEvent();
	void clickEvent(float mouseX, float mouseY);
	void releaseEvent(float mouseX, float mouseY);
	void unselectEvent();
	
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

	D2D1_RECT_F mapToLocal(D2D1_RECT_F rect);
	D2D1_POINT_2F mapToLocal(D2D1_POINT_2F point);
	D2D1_RECT_F mapToGlobal(D2D1_RECT_F rect);
	D2D1_POINT_2F mapToGlobal(D2D1_POINT_2F point);

	D2D1_RECT_F currentRect() const;
	bool createFontFormat();

	void drawBasicShape(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush);
	void basicPlace(int x, int y);
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

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override;

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

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override {};
};

class Checkbox : public Widget
{
public:
	Checkbox(Core* core, const std::string& text = "");
	~Checkbox() = default;
	Checkbox(const Checkbox&) = delete;
	Checkbox(Checkbox&&) = delete;
	Checkbox& operator=(const Checkbox&) = delete;
	Checkbox& operator=(Checkbox&&) = delete;

	void setSize(int width, int height) override;
	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override;

	bool isChecked();

private:
	D2D1_POINT_2F m_boxPoint, m_textPoint;
	D2D1_SIZE_F m_boxSize, m_textSize;
	bool m_check;

	std::array<D2D1_POINT_2F, 3> currentCheckbox();
	void calculateBoxAndTextLayout();
	D2D1_RECT_F currentTextRect() const;
	D2D1_RECT_F currentBoxRect() const;
};

class Textedit : public Widget
{
public:
	Textedit(Core* core);
	~Textedit();
	Textedit(const Textedit&) = delete;
	Textedit(Textedit&&) = delete;
	Textedit& operator=(const Textedit&) = delete;
	Textedit& operator=(Textedit&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override;
	void unselect();

	std::string getText();

	void keyInput(char key);
	void otherKeys(uint32_t key);

private:
	std::vector<float> m_charWidths;
	float m_lineHeight;
	Timer* m_cursorTimer;
	bool m_selected;
	bool m_drawCursor;
	uint32_t m_cursorPosition;

	float calculateCharDimension(char character);
	uint32_t getCursorPosition(float x);
	void updateCursorPosition(bool increase);

	D2D1_RECT_F currentCursorLine() const;
	D2D1_RECT_F currentTextRect() const;

	void switchCursor();
	void restartCursorTimer();
};

class Image : public Widget
{
public:
	Image(Core* core, const std::string& filename = "");
	~Image();
	Image(const Image&) = delete;
	Image(Image&&) = delete;
	Image& operator=(const Image&) = delete;
	Image& operator=(Image&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override {};

	void loadImageFromFile(const std::string& filename);

private:
	ID2D1Bitmap* m_drawingBitmap;
	IWICBitmap* m_wicBitmap;
	uint32_t m_imageWidth, m_imageHeight;
};

class Canvas : public Widget
{
public:
	Canvas(Core* core, int width = 100, int height = 100, const Color& fillColor = { 255, 255, 255 });
	~Canvas();
	Canvas(const Canvas&) = delete;
	Canvas(Canvas&&) = delete;
	Canvas& operator=(const Canvas&) = delete;
	Canvas& operator=(Canvas&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override {};

	void fill(const Color& color, bool autoLock = true);
	void setPixel(const Color& color, int x, int y, bool autoLock = true);
	void setPixel(const Color& color, size_t pos, bool autoLock = true);
	void drawLine(Point p1, Point p2, const Color& color, bool autoLock = true);
	void drawRectangle(Point p1, Point p2, const Color& color, bool autoLock = true);
	void drawTriangle(Point p1, Point p2, Point p3, const Color& color, bool autoLock = true);
	void drawCircle(Point p, int r, const Color& color, bool autoLock = true);
	void drawEllipse(Point p, int ra, int rb, const Color& color, bool autoLock = true);
	void lock();
	void unlock();

private:
	ID2D1Bitmap* m_drawingBitmap;
	IWICBitmap* m_wicBitmap;
	IWICBitmapLock* m_wicLock;
	byte* m_buffer;
	int m_bufferWidth, m_bufferHeight;

	void createPixelBuffer();
	void setColor(const Color& color, size_t pos);
	void checkBounds(int& n) const;
	int bytePosFromXY(int x, int y) const;
	D2D1_RECT_F bufferRect() const;

	void fillBottomTriangle(Point p1, Point p2, Point p3, const Color& color);
	void fillTopTriangle(Point p1, Point p2, Point p3, const Color& color);

	static void swap(int& a, int& b);
	static void swap(Point& a, Point& b);
	static float frac(float a);
	static float rfrac(float a);
};

class Table : public Widget
{
public:
	Table(Core* core);
	~Table() = default;
	Table(const Table&) = delete;
	Table(Table&&) = delete;
	Table& operator=(const Table&) = delete;
	Table& operator=(Table&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override {};

	void setCell(const std::string& text, int row, int col, int rowSpan = 1, int colSpan = 1);
	void setLineWidth(float lineWidth);

private:
	int m_rows, m_cols;
	float m_colWidth, m_rowHeight, m_lineWidth;
	std::vector<GridEntry<std::string>> m_entries;
	std::vector<int> m_blockedEntries;

	void drawTextInCell(int row, int col, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush);
	void drawCellLines(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush);
};

class Combobox : public Widget
{
public:
	Combobox(Core* core);
	~Combobox() = default;
	Combobox(const Combobox&) = delete;
	Combobox(Combobox&&) = delete;
	Combobox& operator=(const Combobox&) = delete;
	Combobox& operator=(Combobox&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override {};

private:
};

}
