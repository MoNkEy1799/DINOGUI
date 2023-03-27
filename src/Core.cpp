#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>

DINOGUI::Base::Base(const std::string& windowName, int width, int height, int x, int y)
    : m_factory(nullptr), m_renderTarget(nullptr), m_colorBrush(nullptr),
    m_windowName(windowName), m_width(width), m_height(height), m_xPos(x), m_yPos(y),
    m_hoverWidget(nullptr), m_clickWidget(nullptr)
{
}

int DINOGUI::Base::run()
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    std::wstring temp = std::wstring(m_windowName.begin(), m_windowName.end());
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

LRESULT DINOGUI::Base::HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam)
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

    default:
        return DefWindowProc(m_windowHandle, messageCode, wParam, lParam);
    }

    return 1;
}

int DINOGUI::Base::createFactoryAndDPI()
{
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory)))
    {
        return -1;
    }

    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_writeFactory)))
    {
        return -1;
    }

    DINOGUI::DPIConverter::Initialize(m_windowHandle);
    return 0;
}

void DINOGUI::Base::destroyWindow()
{
    destroyGraphicsResources();
    DINOGUI::safeReleaseInterface(&m_factory);
    PostQuitMessage(0);
}

void DINOGUI::Base::resizeWindow()
{
    if (m_renderTarget)
    {
        m_renderTarget->Resize(getCurrentWindowSize());
        InvalidateRect(m_windowHandle, NULL, FALSE);
    }
}

void DINOGUI::Base::paintWidgets()
{
    std::cout << "draw call" << std::endl;
    HRESULT hResult = createGraphicsResource();

    if (SUCCEEDED(hResult))
    {
        PAINTSTRUCT painStruct;
        BeginPaint(m_windowHandle, &painStruct);
        m_renderTarget->BeginDraw();
        m_renderTarget->Clear(D2D1::ColorF(0.8f, 0.8f, 0.8f));

        for (DINOGUI::Widget* widget : m_displayWidgets)
        {
            widget->draw(m_renderTarget, m_colorBrush);
        }

        hResult = m_renderTarget->EndDraw();

        if (FAILED(hResult) || hResult == D2DERR_RECREATE_TARGET)
        {
            destroyGraphicsResources();
        }

        EndPaint(m_windowHandle, &painStruct);
    }
}

void DINOGUI::Base::mouseMove(int posX, int posY, DWORD flags)
{
    float x = DPIConverter::PixelsToDips(posX);
    float y = DPIConverter::PixelsToDips(posY);

    DINOGUI::Widget* underMouse = getWidgetUnderMouse(x, y);

    if (m_hoverWidget != underMouse)
    {
        if (underMouse)
        {
            underMouse->setWidgetState(WidgetState::HOVER);
        }
        else
        {
            m_hoverWidget->setWidgetState(WidgetState::NORMAL);
        }

        InvalidateRect(m_windowHandle, nullptr, false);
        m_hoverWidget = underMouse;
    }
}

void DINOGUI::Base::leftClick(int posX, int posY, DWORD flags)
{
}

D2D1_SIZE_U DINOGUI::Base::getCurrentWindowSize()
{
    RECT rect;
    GetClientRect(m_windowHandle, &rect);
    return D2D1::SizeU(rect.right, rect.bottom);
}

DINOGUI::Widget* DINOGUI::Base::getWidgetUnderMouse(int x, int y)
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

HRESULT DINOGUI::Base::createGraphicsResource()
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
            hResult = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f), &m_colorBrush);
        }
    }

    return hResult;
}

void DINOGUI::Base::destroyGraphicsResources()
{
    DINOGUI::safeReleaseInterface(&m_renderTarget);
    DINOGUI::safeReleaseInterface(&m_colorBrush);
}
