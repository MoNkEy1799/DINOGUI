#pragma once
#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(linker, "/ENTRY:mainCRTStartup")
#define NOMINMAX

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
#include <map>

namespace DINOGUI
{

class LayoutObject;
class Container;
class Core;
class CoreInterface;
class Text;
class Timer;
class Widget;
class Button;
class Label;
class Checkbox;
class Textedit;
class Image;
class Canvas;
class Table;
class Combobox;
class Slider;
enum class LayoutObjectType;
enum class WidgetState;
enum class WidgetType;

enum class LayoutObjectType { WIDGET, CONTAINER };

class LayoutObject
{
public:
	const LayoutObjectType layoutType;

protected:
	LayoutObject(LayoutObjectType type) : layoutType(type) {}
};

class Container : public LayoutObject
{
public:
	Container(Core* core);
	~Container();
	Container(const Container&) = delete;
	Container(Container&&) = delete;
	Container& operator=(const Container&) = delete;
	Container& operator=(Container&&) = delete;

	void addWidget(Widget* widget, int row, int col, int rowSpan = 1, int colSpan = 1);
	void addContainer(Container* container, int row, int col, int rowSpan = 1, int colSpan = 1);

private:
	Core* m_core;
	std::vector<GridEntry<LayoutObject*>> m_objects;
	std::vector<Container*> m_containers;
	std::array<float, 4> m_margins;
	std::array<float, 2> m_spacing;
	int m_rows, m_cols;

	void updatePositionAndSizes();
};

class Core : public TemplateWindow<Core>
{
public:
	Core(const std::string& windowName = "DINOGUI", int width = 200, int height = 200, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
	int run();
	void setFixedWindowSize(int width, int height);
	void setMinimumWindowSize(int width, int height);
	void setMaximumWindowSize(int width, int height);
	Size<int> getCurrentWindowSize() const;
	void redrawScreen() const;

private:
	friend class TemplateWindow;
	friend class CoreInterface;

	static int m_instanceCounter;
	LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam);

	ID2D1Factory* m_factory;
	IDWriteFactory* m_writeFactory;
	IWICImagingFactory* m_imageFactory;
	ID2D1HwndRenderTarget* m_renderTarget;
	ID2D1SolidColorBrush* m_colorBrush;
	D2D1_POINT_2F m_mousePosition;

	std::vector<Widget*> m_widgets;
	std::vector<Widget*> m_displayWidgets;
	std::vector<Container*> m_containers;
	std::vector<Timer*> m_timers;
	Widget* m_hoverWidget;
	Widget* m_clickWidget;
	Widget* m_selectWidget;
	Size<int> m_size, m_minSize, m_maxSize;
	std::string m_windowName;
	int m_xPos, m_yPos;
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

	Widget* getWidgetUnderMouse(float x, float y) const;

	HRESULT	createGraphicsResource();
	void destroyGraphicsResources();
};

class CoreInterface
{
protected:
	CoreInterface() {}

	static HWND getWindowHandle(Core* core);
	static ID2D1Factory* getFactory(Core* core);
	static IDWriteFactory* getWriteFactory(Core* core);
	static IWICImagingFactory* getImageFactory(Core* core);

	template<typename T>
	static void addToVector(std::vector<T>& vec, T object)
	{
		if (std::find(vec.begin(), vec.end(), object) == vec.end())
		{
			vec.push_back(object);
		}
	}
	template<typename T>
	static void removeFromVector(std::vector<T>& vec, T object)
	{
		vec.erase(std::remove(vec.begin(), vec.end(), object), vec.end());
	}

	static void addWidget(Core* core, Widget* widget);
	static void removeWidget(Core* core, Widget* widget);
	static void addDisplayWidget(Core* core, Widget* widget);
	static void removeDisplayWidget(Core* core, Widget* widget);
	static void addContainer(Core* core, Container* container);
	static void removeContainer(Core* core, Container* container);
	static void addTimer(Core* core, Timer* timer);
	static void removeTimer(Core* core, Timer* timer);

	static void setHoverWidget(Core* core, Widget* widget);
	static void setClickWidget(Core* core, Widget* widget);
	static void setSelectWidget(Core* core, Widget* widget);
};

class Text : protected CoreInterface
{
public:
	Text(Core* core, const std::string& text);
	~Text();

	void draw(D2D1_RECT_F rectangle, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush);
	void setText(const std::string& text);
	void setFont(const Font& font);
	void setColor(const Color& color);
	void unsetColor();
	void setAlignment(Alignment align);

	std::string& getText();
	IDWriteTextFormat* getFontFormat();
	bool fontFormatChanged;

private:
	IDWriteTextFormat* m_fontFormat;
	Core* m_core;
	std::string m_text;
	Font m_font;
	DWRITE_TEXT_ALIGNMENT m_hAlign;
	DWRITE_PARAGRAPH_ALIGNMENT m_vAlign;
	Color m_color;
	bool m_colorSet;

	bool createFontFormat();
};

class Timer : protected CoreInterface
{
public:
	Timer(Core* core, uint32_t timeout = 1000, std::function<void()> callback = nullptr);
	~Timer();
	Timer(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer& operator=(Timer&&) = delete;

	void start();
	void stop();
	void restart();
	bool isActive();

	uint32_t timeoutDelay;
	std::function<void()> callback;

private:
	static void timerFunction(HWND, uint32_t, uint64_t classPtr, DWORD);

	Core* m_core;
	bool m_active;
};

enum class WidgetState { NORMAL, HOVER, CLICKED, SELECTED, SELECTED_HOVER, CHECKED, CHECKED_HOVER };
enum class WidgetType { NONE, BUTTON, LABEL, CHECKBOX, TEXTEDIT, IMAGE, CANVAS, TABLE, COMBOBOX, SLIDER };

class Widget : public LayoutObject, protected CoreInterface
{
public:
	Widget(Core* core);
	virtual ~Widget();
	Widget(const Widget&) = delete;
	Widget(Widget&&) = delete;
	Widget& operator=(const Widget&) = delete;
	Widget& operator=(Widget&&) = delete;

	virtual void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) = 0;
	virtual void place(int x, int y) = 0;
	void centerPlace(int x, int y);
	virtual void clicked(float mouseX, float mouseY) = 0;
	
	void show();
	void hide();
	void drawBorder(bool draw = true);
	void drawBackground(bool draw = true);
	void setTheme(ColorTheme* theme);
	virtual void resize(int width, int height);
	void setFixedSize(int width, int height);
	void setMinimumSize(int width, int height);
	void setMaximumSize(int width, int height);
	ResizeState getResizeState();

	WidgetType getWidgetType() const;
	bool contains(float x, float y) const;

	void receiveEvent(Event* event);
	void enterEvent();
	void leaveEvent();
	void clickEvent(float mouseX, float mouseY);
	void holdEvent(float mouseX, float mouseY);
	void releaseEvent(float mouseX, float mouseY);
	void unselectEvent();
	
protected:
	Core* m_core;
	ColorTheme* m_theme;
	WidgetState m_state;
	WidgetType m_type;
	D2D1_POINT_2F m_point;
	Size<float> m_size, m_minSize, m_maxSize;
	ResizeState m_resizeState;
	bool m_drawBackground, m_drawBorder;
	bool m_hoverable, m_clickable, m_holdable, m_selectable, m_checkable;
	bool m_checked, m_selected;

	D2D1_RECT_F currentRect() const;
	void basicDrawBackgroundBorder(const D2D1_RECT_F& rect, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush);
	void basicPlace(int x, int y);
};

class Button : public Widget
{
public:
	Button(Core* core, const std::string& text = "", std::function<void()> function = nullptr);
	~Button();
	Button(const Button&) = delete;
	Button(Button&&) = delete;
	Button& operator=(const Button&) = delete;
	Button& operator=(Button&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override;

	void connect(std::function<void()> function);
	Text* getTextWidget();
	void setCheckable(bool check = true);
	bool isChecked();

private:
	std::function<void()> m_clickFunction;
	Text* m_text;
};

class Label : public Widget
{
public:
	Label(Core* core, const std::string& text = "");
	~Label();
	Label(const Label&) = delete;
	Label(Label&&) = delete;
	Label& operator=(const Label&) = delete;
	Label& operator=(Label&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override {};

	Text* getTextWidget();
	void setText(const std::string& text);

private:
	Text* m_text;
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

	void resize(int width, int height) override;
	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override {};

	Text* getTextWidget();
	bool isChecked() const;

private:
	Text* m_text;
	D2D1_POINT_2F m_boxPoint, m_textPoint;
	Size<float> m_boxSize, m_textSize;

	std::array<D2D1_POINT_2F, 3> currentCheckbox() const;
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
	void stopCursorTimer();

	std::string getText() const;
	Text* getTextWidget(const std::string& which = "main");
	void setPlaceholderText(const std::string& text);

	void keyInput(char key);
	void otherKeys(uint32_t key);

private:
	Text* m_text;
	Text* m_placeholder;
	Text* m_cutoffText;
	std::vector<float> m_charWidths;
	float m_lineHeight;
	Timer* m_cursorTimer;
	bool m_drawCursor, m_trailing;
	uint32_t m_cursorPosition;

	float calculateCharDimension(char character);
	uint32_t getCursorPosition(float x) const;
	void updateCursorPosition(bool increase);

	D2D1_RECT_F currentCursorLine() const;

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

	D2D1_RECT_F bitmapRect() const;
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

	void antialias(bool b, float thickness = 2.0f);
	void fill(const Color& color);
	void setPixel(const Color& color, int x, int y);
	void setPixel(const Color& color, size_t pos);
	void drawLine(Point<float> p1, Point<float> p2, const Color& color);
	void drawRectangle(Point<float> p1, Point<float> p2, const Color& color);
	void drawTriangle(Point<float> p1, Point<float> p2, Point<float> p3, const Color& color);
	void drawCircle(Point<float> p, int r, const Color& color);
	void drawEllipse(Point<float> p, int ra, int rb, const Color& color);

private:
	ID2D1Bitmap* m_drawingBitmap;
	IWICBitmap* m_wicBitmap;
	IWICBitmapLock* m_wicLock;
	byte* m_buffer;
	int m_bufferWidth, m_bufferHeight;
	bool m_antialias, m_twoPartTriangle;
	float m_thickness;
	std::array<Point<float>, 4> m_triangle;
	std::array<float, 3> m_bisectors;

	void createPixelBuffer();
	void lockBuffer();
	void unlockBuffer();
	void setColor(const Color& color, size_t bytePos);
	void fillBottomTriangle(Point<float> p1, Point<float> p2, Point<float> p3, const Color& color);
	void fillTopTriangle(Point<float> p1, Point<float> p2, Point<float> p3, const Color& color);
	
	bool inBounds(int x, int y) const;
	void checkBounds(float& x, float& y) const;
	size_t bytePosFromXY(int x, int y) const;
	D2D1_RECT_F bufferRect() const;
	
	static float length(Point<float> p1, Point<float> p2);
	static float distance(Point<float> p, Point<float> l1, Point<float> l2);
	static Point<float> lineIntersect(float a, float c, float b, float d);
	static Point<float> midPoint(Point<float> p1, Point<float> p2);
	static float gradient(Point<float> p1, Point<float> p2);
	static float invGradient(Point<float> p1, Point<float> p2);
	static void swap(int& a, int& b);
	static void swap(Point<float>& a, Point<float>& b);
};

class Table : public Widget
{
public:
	Table(Core* core);
	~Table();
	Table(const Table&) = delete;
	Table(Table&&) = delete;
	Table& operator=(const Table&) = delete;
	Table& operator=(Table&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override {};

	void setCell(const std::string& text, int row, int col, int rowSpan = 1, int colSpan = 1);
	std::vector<Text*> getTextWidgets();
	Text* getTextWidget(int row, int col);
	void setRowWeight(int row, float weight);
	void setColWeight(int col, float weight);
	void setLineWidth(float lineWidth);

private:
	int m_rows, m_cols;
	float m_lineWidth, m_prevBorderWidth;
	std::vector<GridEntry<Text*>> m_entries;
	std::vector<int> m_blockedEntries;
	std::vector<float> m_rowWeights, m_rowHeights, m_colWeights, m_colWidths;

	void drawTextInCell(int row, int col, ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush);
	void drawCellLines(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush);
	void calculateDimensions();
};

class Combobox : public Widget
{
public:
	Combobox(Core* core, const std::string& text = "");
	~Combobox();
	Combobox(const Combobox&) = delete;
	Combobox(Combobox&&) = delete;
	Combobox& operator=(const Combobox&) = delete;
	Combobox& operator=(Combobox&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override;
	bool dropdownContains(float x, float y) const;
	void setHoverIndex(float x, float y);
	void setDropdown(bool drop = true);

	void addItem(const std::string& text);
	void insertItem(const std::string& text, int index);
	void removeItem(int index);
	void changeUnfoldDirection(bool upward = true);
	std::string getCurrentText() const;
	int getCurrentIndex() const;
	std::vector<Text*> getTextWidgets();
	Text* getTextWidget(int index);

private:
	std::vector<Text*> m_boxText;
	int m_currentIndex, m_hoverIndex;
	bool m_dropdown, m_upward;

	std::array<D2D1_POINT_2F, 3> getArrowPoints() const;
	int indexUnderMouse(float x, float y) const;
	void drawDropdown(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush);
};

class Slider : public Widget
{
public:
	Slider(Core* core, bool vertical = false);
	~Slider() = default;
	Slider(const Slider&) = delete;
	Slider(Slider&&) = delete;
	Slider& operator=(const Slider&) = delete;
	Slider& operator=(Slider&&) = delete;

	void draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush) override;
	void place(int x, int y) override;
	void clicked(float mouseX, float mouseY) override;

	void setVertical(bool vertical = true);
	void setMaxTicks(int ticks);
	int getCurrentTick();
	float getCurrentPercentage();

private:
	int m_ticks, m_currentTick;
	bool m_vertical;
};

}
