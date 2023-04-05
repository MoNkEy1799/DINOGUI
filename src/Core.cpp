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
    : m_factory(nullptr), m_writeFactory(nullptr), m_renderTarget(nullptr), m_colorBrush(nullptr), m_strokeStyle(nullptr),
      m_windowName(windowName), m_width(width), m_height(height), m_xPos(x), m_yPos(y), m_mousePosition({ 0.0f, 0.0f }),
      m_hoverWidget(nullptr), m_clickWidget(nullptr), m_selectedWidget(nullptr)
{
}

int Core::run()
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    std::wstring temp(m_windowName.begin(), m_windowName.end());

    if (!createWindow(temp.c_str(), WS_OVERLAPPEDWINDOW, m_width, m_height, m_xPos, m_yPos))
    {
        throw std::runtime_error("Could not create Window");
    }

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

    case WM_MOUSEMOVE:
        mouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        return 0;

    case WM_LBUTTONDOWN:
        leftClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        return 0;

    case WM_LBUTTONUP:
        leftRelease(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
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

    DPIConverter::Initialize(m_windowHandle);
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
    HRESULT hResult = createGraphicsResource();

    if (SUCCEEDED(hResult))
    {
        PAINTSTRUCT painStruct;
        BeginPaint(m_windowHandle, &painStruct);
        m_renderTarget->BeginDraw();
        m_renderTarget->Clear(toD2DColorF(DINOCOLOR_WINDOW_LIGHT));
        m_renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

        for (Widget* widget : m_displayWidgets)
        {
            widget->draw(m_renderTarget, m_colorBrush, m_strokeStyle);
        }

        hResult = m_renderTarget->EndDraw();

        if (FAILED(hResult) || hResult == D2DERR_RECREATE_TARGET)
        {
            destroyGraphicsResources();
        }

        EndPaint(m_windowHandle, &painStruct);
    }
}

void Core::mouseMove(int posX, int posY, DWORD flags)
{
    int x = DPIConverter::PixelsToDips(posX);
    int y = DPIConverter::PixelsToDips(posY);

    if (flags & DINOGUI_ALL_MOUSE_BUTTONS)
    {
        return;
    }

    Widget* underMouse = getWidgetUnderMouse(x, y);
    if (m_hoverWidget != underMouse)
    {
        if (underMouse)
        {
            underMouse->enterEvent();
        }
        if (m_hoverWidget)
        {
            m_hoverWidget->leaveEvent();
        }
        m_hoverWidget = underMouse;
    }
}

void Core::leftClick(int posX, int posY, DWORD flags)
{
    int x = DPIConverter::PixelsToDips(posX);
    int y = DPIConverter::PixelsToDips(posY);

    //std::cout << "Mouse Pos: " << x << " | " << y << std::endl;

    Widget* underMouse = getWidgetUnderMouse(x, y);
    if (!underMouse)
    {
        if (m_selectedWidget)
        {
            m_selectedWidget->clickEvent();
        }
        m_clickWidget = nullptr;
        m_selectedWidget = nullptr;
        return;
    }

    if (underMouse == m_selectedWidget)
    {
        return;
    }
    else if (m_selectedWidget)
    {
        m_selectedWidget->clickEvent();
    }
    m_selectedWidget = nullptr;
    underMouse->clickEvent();
    m_clickWidget = underMouse;
}

void Core::leftRelease(int posX, int posY, DWORD flags)
{
    int x = DPIConverter::PixelsToDips(posX);
    int y = DPIConverter::PixelsToDips(posY);

    Widget* underMouse = getWidgetUnderMouse(x, y);
    if (!underMouse)
    {
        if (m_clickWidget)
        {
            m_clickWidget->leaveEvent();
        }
        m_hoverWidget = nullptr;
        m_clickWidget = nullptr;
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
    m_hoverWidget = underMouse;
    m_clickWidget = nullptr;
}

D2D1_SIZE_U Core::getCurrentWindowSize() const
{
    RECT rect;
    GetClientRect(m_windowHandle, &rect);
    return D2D1::SizeU(rect.right, rect.bottom);
}

Widget* Core::getWidgetUnderMouse(int x, int y) const
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
            hResult = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f), &m_colorBrush);
        }
    }

    if (!m_strokeStyle)
    {
        D2D1_STROKE_STYLE_PROPERTIES props =
        {
            D2D1_CAP_STYLE_SQUARE,
            D2D1_CAP_STYLE_SQUARE,
            D2D1_CAP_STYLE_SQUARE,
            D2D1_LINE_JOIN_MITER,
            0.0f,
            D2D1_DASH_STYLE_SOLID,
            0.0f
        };
        hResult = m_factory->CreateStrokeStyle(props, nullptr, 0, &m_strokeStyle);
    }

    return hResult;
}

void Core::destroyGraphicsResources()
{
    safeReleaseInterface(&m_renderTarget);
    safeReleaseInterface(&m_colorBrush);
    safeReleaseInterface(&m_strokeStyle);
}
