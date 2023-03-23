#include "BaseWindow.h"
#include "Widget.h"

#include <Windows.h>
#include <d2d1.h>
#include <iostream>

DINOGUI::Base::Base(const std::string& windowName)
    : m_factory(nullptr), m_renderTarget(nullptr), m_colorBrush(nullptr), m_windowName(windowName), m_pcwName(nullptr)
{
}

int DINOGUI::Base::run()
{
    if (!createWindow(getWindowName(), WS_OVERLAPPEDWINDOW))
    {
        return -1;
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
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory)))
        {
            return -1;
        }
        return 0;

    case WM_DESTROY:
        destroyGraphicsResources();
        DINOGUI::safeReleaseInterface(&m_factory);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        paintChildren();
        return 0;

    default:
        return DefWindowProc(m_windowHandle, messageCode, wParam, lParam);
    }

    return 1;
}

void DINOGUI::Base::paintChildren()
{
    HRESULT hResult = createGraphicsResource();

    if (SUCCEEDED(hResult))
    {
        PAINTSTRUCT painStruct;
        BeginPaint(m_windowHandle, &painStruct);
        m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 1.0f, 1.0f), &m_colorBrush);
        m_renderTarget->BeginDraw();
        m_renderTarget->Clear(D2D1::ColorF(0.8f, 0.8f, 0.8f));
        std::cout << "clear" << std::endl;

        m_renderTarget->FillRectangle(D2D1::RectF(50.0f, 50.0f, 70.0f, 70.0f), m_colorBrush);

        /*for (DINOGUI::Widget* child : m_childWidgets)
        {
            child->draw(m_renderTarget, m_colorBrush);
        }*/

        hResult = m_renderTarget->EndDraw();

        if (FAILED(hResult) || hResult == D2DERR_RECREATE_TARGET)
        {
            destroyGraphicsResources();
        }

        EndPaint(m_windowHandle, &painStruct);
    }
}

PCWSTR DINOGUI::Base::getWindowName()
{
    if (!m_pcwName)
    {
        std::wstring temp = std::wstring(m_windowName.begin(), m_windowName.end());
        m_pcwName = PCWSTR(temp.c_str());
    }
    return m_pcwName;
}

HRESULT DINOGUI::Base::createGraphicsResource()
{
    HRESULT result = S_OK;

    if (!m_renderTarget)
    {
        RECT rect;
        GetClientRect(m_windowHandle, &rect);
        D2D1_SIZE_U windowSize = D2D1::SizeU(rect.right, rect.bottom);

        result = m_factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_windowHandle, windowSize),
            &m_renderTarget);
    }

    return result;
}

void DINOGUI::Base::destroyGraphicsResources()
{
    DINOGUI::safeReleaseInterface(&m_renderTarget);
    DINOGUI::safeReleaseInterface(&m_colorBrush);
}
