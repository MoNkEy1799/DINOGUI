#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <string>

using namespace DINOGUI;

void Core::DEBUG_DRAW_RECT(D2D1_RECT_F r)
{
    m_colorBrush->SetColor(D2D1::ColorF(1.0f, 0.0f, 0.0f));
    m_renderTarget->DrawRectangle(r, m_colorBrush);
}

Core::Core(const std::string& windowName, int width, int height, int x, int y)
    : m_factory(nullptr), m_writeFactory(nullptr), m_renderTarget(nullptr), m_colorBrush(nullptr),
    m_windowName(windowName), m_width(width), m_height(height), m_xPos(x), m_yPos(y), m_mousePosition({ 0.0f, 0.0f }),
    m_hoverWidget(nullptr), m_clickWidget(nullptr), m_selectedWidget(nullptr), m_changeCursor(true)
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    std::wstring temp(m_windowName.begin(), m_windowName.end());

    throwIfFailed(createWindow(temp.c_str(), WS_OVERLAPPEDWINDOW, m_width, m_height, m_xPos, m_yPos), "Failed to create window");
}

int Core::run()
{
    ShowWindow(m_windowHandle, SW_SHOW);

    MSG message;
    while (GetMessage(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return 0;
}

LRESULT Core::HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam)
{
    switch (messageCode)
    {
    case WM_CREATE:
        return createFactoryAndDPI();

    case WM_DESTROY:
        destroyWindow();
        return 0;

    case WM_PAINT:
        paintWidgets();
        return 0;

    case WM_SIZE:
        resizeWindow();
        return 0;

    /*case WM_DPICHANGED:
        DPIHandler::Initialize(m_windowHandle);
        std::cout << "DPI " << GetDpiForWindow(m_windowHandle) << std::endl;
        redrawScreen();
        return 0;*/

    case WM_SETCURSOR:
        setCursor();
        return 0;

    case WM_MOUSEMOVE:
        mouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        return 0;

    case WM_LBUTTONDOWN:
        leftClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        return 0;

    case WM_LBUTTONUP:
        leftRelease(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        return 0;

    case WM_CHAR:
        processKeys((char)wParam);
        return 0;

    case WM_KEYDOWN:
        processOtherKeys((uint32_t)wParam);
        return 0;

    default:
        return DefWindowProc(m_windowHandle, messageCode, wParam, lParam);
    }

    return 1;
}

void Core::addWidget(Widget* widget)
{ 
    if (std::find(m_widgets.begin(), m_widgets.end(), widget) == m_widgets.end())
    {
        m_widgets.push_back(widget);
    }
}

void Core::removeWidget(Widget* widget)
{
    m_widgets.erase(std::remove(m_widgets.begin(), m_widgets.end(), widget), m_widgets.end());
}

void Core::addDisplayWidget(Widget* widget)
{
    if (std::find(m_displayWidgets.begin(), m_displayWidgets.end(), widget) == m_displayWidgets.end())
    {
        m_displayWidgets.push_back(widget);
    }
}

void Core::removeDisplayWidget(Widget* widget)
{
    m_displayWidgets.erase(std::remove(m_displayWidgets.begin(), m_displayWidgets.end(), widget), m_displayWidgets.end());
}

int Core::createFactoryAndDPI()
{
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory)))
    {
        return -1;
    }

    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_writeFactory)))
    {
        return -1;
    }

    DPIHandler::Initialize(m_windowHandle);
    return 0;
}

void Core::destroyWindow()
{
    while (!m_widgets.empty())
    {
        delete m_widgets.back();
    }
    destroyGraphicsResources();
    safeReleaseInterface(&m_factory);
    safeReleaseInterface(&m_writeFactory);
    PostQuitMessage(0);
}

void Core::resizeWindow()
{
    if (m_renderTarget)
    {
        m_renderTarget->Resize(getCurrentWindowSize());
        redrawScreen();
    }
}

void Core::paintWidgets()
{
    DEBUG_DrawCalls += 1;
    std::cout << "total draw calls: " << DEBUG_DrawCalls << std::endl;
    throwIfFailed(createGraphicsResource(), "Failed to create graphics resources");
    
    PAINTSTRUCT painStruct;
    BeginPaint(m_windowHandle, &painStruct);
    m_renderTarget->BeginDraw();
    m_renderTarget->Clear(toD2DColorF(DINOCOLOR_WINDOW_LIGHT));
    m_renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

    for (Widget* widget : m_displayWidgets)
    {
        widget->draw(m_renderTarget, m_colorBrush);
    }

    HRESULT hResult = m_renderTarget->EndDraw();

    if (FAILED(hResult) || hResult == D2DERR_RECREATE_TARGET)
    {
        destroyGraphicsResources();
    }

    EndPaint(m_windowHandle, &painStruct);
}

void Core::setCursor()
{
    if (!m_changeCursor)
    {
        return;
    }

    if (m_hoverWidget && Widget::selectableWidget(m_hoverWidget->getWidgetType()))
    {
        SetCursor(LoadCursor(NULL, IDC_IBEAM));
        m_changeCursor = false;
    }
    else
    {
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        m_changeCursor = false;
    }
}

void Core::mouseMove(int posX, int posY, DWORD flags)
{
    float x = DPIHandler::PixelsToDips((float)posX);
    float y = DPIHandler::PixelsToDips((float)posY);

    if (flags & DINOGUI_ALL_MOUSE_BUTTONS)
    {
        return;
    }

    Widget* underMouse = getWidgetUnderMouse(x, y);
    if (m_hoverWidget != underMouse)
    {
        m_changeCursor = true;
        if (m_hoverWidget)
        {
            m_hoverWidget->leaveEvent();
        }
        if (underMouse)
        {
            underMouse->enterEvent();
        }
    }
}

void Core::leftClick(int posX, int posY, DWORD flags)
{
    float x = DPIHandler::PixelsToDips((float)posX);
    float y = DPIHandler::PixelsToDips((float)posY);

    //std::cout << "Mouse Pos: " << x << " | " << y << std::endl;

    Widget* underMouse = getWidgetUnderMouse(x, y);
    if (!underMouse)
    {
        if (m_selectedWidget)
        {
            m_selectedWidget->unselectEvent();
        }
        return;
    }
    if (underMouse == m_selectedWidget)
    {
        return;
    }
    if (m_selectedWidget)
    {
        m_selectedWidget->unselectEvent();
    }
    underMouse->clickEvent();
}

void Core::leftRelease(int posX, int posY, DWORD flags)
{
    float x = DPIHandler::PixelsToDips((float)posX);
    float y = DPIHandler::PixelsToDips((float)posY);

    Widget* underMouse = getWidgetUnderMouse(x, y);
    if (!underMouse)
    {
        if (m_clickWidget)
        {
            m_clickWidget->leaveEvent();
        }
        return;
    }
    if (underMouse == m_selectedWidget)
    {
        return;
    }

    if (underMouse == m_clickWidget)
    {
        underMouse->releaseEvent();
    }
    else if (m_clickWidget)
    {
        m_clickWidget->leaveEvent();
    }
    underMouse->enterEvent();
}

void Core::processKeys(char key)
{
    if (m_selectedWidget && m_selectedWidget->getWidgetType() == WidgetType::TEXTEDIT)
    {
        dynamic_cast<Textedit*>(m_selectedWidget)->keyInput(key);
    }
}

void Core::processOtherKeys(uint32_t key)
{
    if (m_selectedWidget && m_selectedWidget->getWidgetType() == WidgetType::TEXTEDIT)
    {
        dynamic_cast<Textedit*>(m_selectedWidget)->otherKeys(key);
    }
}

D2D1_SIZE_U Core::getCurrentWindowSize() const
{
    RECT rect;
    GetClientRect(m_windowHandle, &rect);
    return D2D1::SizeU(rect.right, rect.bottom);
}

Widget* Core::getWidgetUnderMouse(float x, float y) const
{
    for (Widget* widget : m_displayWidgets)
    {
        if (widget->contains(x, y))
        {
            return widget;
        }
    }

    return nullptr;
}

HRESULT Core::createGraphicsResource()
{
    HRESULT hResult = S_OK;

    if (!m_renderTarget)
    {
        hResult = m_factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_windowHandle, getCurrentWindowSize()),
            &m_renderTarget);

        if (SUCCEEDED(hResult))
        {
            hResult = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(0), &m_colorBrush);
        }
    }
    return hResult;
}

void Core::destroyGraphicsResources()
{
    safeReleaseInterface(&m_renderTarget);
    safeReleaseInterface(&m_colorBrush);
}
