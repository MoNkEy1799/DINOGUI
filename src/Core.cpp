#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <string>
#include <limits>

constexpr int MAX_WIN = 100000;

using namespace DINOGUI;

int Core::m_instanceCounter = 0;

Core::Core(const std::string& windowName, int width, int height, int x, int y)
    : m_factory(nullptr), m_writeFactory(nullptr), m_imageFactory(nullptr), m_renderTarget(nullptr),
      m_colorBrush(nullptr), m_xPos(x), m_yPos(y), m_mousePosition({ 0.0f, 0.0f }),
      m_windowName(windowName), m_hoverWidget(nullptr), m_clickWidget(nullptr), m_selectWidget(nullptr),
      m_changeCursor(true), m_minSize({ 0, 0 }), m_size({ width, height }), m_maxSize({ MAX_WIN, MAX_WIN }),
      resizeState(m_size, m_minSize, m_maxSize)
{
    if (++m_instanceCounter > 1)
    {
        throwIfFailed(true, "Cannot create more than one instance of DINOGUI::Core");
    }
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    std::wstring temp(m_windowName.begin(), m_windowName.end());

    Size<int> adjusted = adjustedWindowSize(width, height);
    throwIfFailed(createWindow(temp.c_str(), WS_OVERLAPPEDWINDOW, adjusted.width, adjusted.height,
        m_xPos, m_yPos), "Failed to create window");
}

int Core::run()
{
    ShowWindow(m_windowHandle, SW_SHOW);

    MSG message;
    while (GetMessage(&message, nullptr, 0, 0))
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
        return createFactories();

    case WM_DESTROY:
        destroyWindow();
        return 0;

    case WM_PAINT:
        paintWidgets();
        return 0;

    case WM_SIZE:
        resizeWindow();
        return 0;

    case WM_DPICHANGED:
        DPIHandler::Initialize(m_windowHandle);
        std::cout << "DPI " << GetDpiForWindow(m_windowHandle) << std::endl;
        redrawScreen();
        return 0;

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

void Core::redrawScreen() const
{
    InvalidateRect(m_windowHandle, nullptr, false);
}

int Core::createFactories()
{
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory)))
    {
        return -1;
    }

    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_writeFactory)))
    {
        return -1;
    }

    if (FAILED(CoInitialize(nullptr)))
    {
        return -1;
    }

    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_imageFactory))))
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
    while (!m_containers.empty())
    {
        delete m_containers.back();
    }
    while (!m_timers.empty())
    {
        delete m_timers.back();
    }
    destroyGraphicsResources();
    safeReleaseInterface(&m_factory);
    safeReleaseInterface(&m_writeFactory);
    safeReleaseInterface(&m_imageFactory);
    PostQuitMessage(0);
}

void Core::resizeWindow()
{
    if (m_renderTarget)
    {
        Size<int> size = getCurrentWindowSize();
        m_renderTarget->Resize({ (uint32_t)size.width, (uint32_t)size.height });
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
    m_renderTarget->Clear(Color::d2d1(DINOCOLOR_WINDOW_LIGHT));
    m_renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

    for (Widget* widget : m_displayWidgets)
    {
        widget->draw(m_renderTarget, m_colorBrush);
    }
    if (m_selectWidget && m_selectWidget->getWidgetType() == WidgetType::COMBOBOX)
    {
        dynamic_cast<Combobox*>(m_selectWidget)->draw(m_renderTarget, m_colorBrush);
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

    if (m_hoverWidget && m_hoverWidget->getWidgetType() == WidgetType::TEXTEDIT)
    {
        SetCursor(LoadCursor(nullptr, IDC_IBEAM));
        m_changeCursor = false;
    }
    else
    {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        m_changeCursor = false;
    }
}

void Core::mouseMove(int posX, int posY, DWORD flags)
{
    float x = DPIHandler::PixelsToDips((float)posX);
    float y = DPIHandler::PixelsToDips((float)posY);

    if (flags & MK_LBUTTON)
    {
        if (m_clickWidget)
        {
            m_clickWidget->receiveEvent(new Event(EventType::HOLD_EVENT, x, y));
        }
        return;
    }
    else if (flags & DINOGUI_ALL_MOUSE_BUTTONS)
    {
        return;
    }

    Widget* underMouse = getWidgetUnderMouse(x, y);
    if (m_hoverWidget != underMouse)
    {
        m_changeCursor = true;
        if (m_hoverWidget)
        {
            m_hoverWidget->receiveEvent(new Event(EventType::LEAVE_EVENT, x, y));
        }
        if (underMouse)
        {
            underMouse->receiveEvent(new Event(EventType::ENTER_EVENT, x, y));
        }
    }
    else if (underMouse && underMouse->getWidgetType() == WidgetType::COMBOBOX)
    {
        dynamic_cast<Combobox*>(underMouse)->setHoverIndex(x, y);
    }
}

void Core::leftClick(int posX, int posY, DWORD flags)
{
    float x = DPIHandler::PixelsToDips((float)posX);
    float y = DPIHandler::PixelsToDips((float)posY);

    Widget* underMouse = getWidgetUnderMouse(x, y);
    if (!underMouse)
    {
        if (m_selectWidget)
        {
            m_selectWidget->receiveEvent(new Event(EventType::UNSELECT_EVENT, x, y));
        }
        return;
    }
    if (underMouse == m_selectWidget)
    {
        m_selectWidget->receiveEvent(new Event(EventType::CLICK_EVENT, x, y));
        return;
    }
    if (m_selectWidget)
    {
        m_selectWidget->receiveEvent(new Event(EventType::UNSELECT_EVENT, x, y));
    }
    underMouse->receiveEvent(new Event(EventType::CLICK_EVENT, x, y));
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
            m_clickWidget->receiveEvent(new Event(EventType::LEAVE_EVENT, x, y));
        }
        return;
    }
    if (underMouse == m_selectWidget)
    {
        return;
    }
    if (underMouse == m_clickWidget)
    {
        underMouse->receiveEvent(new Event(EventType::REALEASE_EVENT, x, y));
    }
    else if (m_clickWidget)
    {
        m_clickWidget->receiveEvent(new Event(EventType::LEAVE_EVENT, x, y));
    }
    underMouse->receiveEvent(new Event(EventType::ENTER_EVENT, x, y));
}

void Core::processKeys(char key)
{
    if (m_selectWidget && m_selectWidget->getWidgetType() == WidgetType::TEXTEDIT)
    {
        dynamic_cast<Textedit*>(m_selectWidget)->keyInput(key);
    }
}

void Core::processOtherKeys(uint32_t key)
{
    if (m_selectWidget && m_selectWidget->getWidgetType() == WidgetType::TEXTEDIT)
    {
        dynamic_cast<Textedit*>(m_selectWidget)->otherKeys(key);
    }
}

Size<int> Core::adjustedWindowSize(int width, int height)
{
    RECT windowRect = { 0, 0, width, height };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);
    long adjustedWidth = windowRect.right - windowRect.left;
    long adjustedHeight = windowRect.bottom - windowRect.top;
    return { windowRect.right - windowRect.left, windowRect.bottom - windowRect.top };
}

void Core::setFixedWindowSize(int width, int height)
{
    m_minSize = { limitRange(width, 0, MAX_WIN), limitRange(height, 0, MAX_WIN) };
    m_maxSize = { limitRange(width, 0, MAX_WIN), limitRange(height, 0, MAX_WIN) };
}

void Core::setMinimumWindowSize(int width, int height)
{
    m_minSize = { limitRange(width, 0, m_maxSize.width), limitRange(height, 0, m_maxSize.height) };
}

void Core::setMaximumWindowSize(int width, int height)
{
    m_maxSize = { limitRange(width, m_minSize.width, MAX_WIN), limitRange(height, m_minSize.height, MAX_WIN) };
}

Widget* Core::getWidgetUnderMouse(float x, float y) const
{
    if (m_selectWidget && m_selectWidget->getWidgetType() == WidgetType::COMBOBOX)
    {
        if (dynamic_cast<const Combobox*>(m_selectWidget)->dropdownContains(x, y))
        {
            return m_selectWidget;
        }
    }
    for (auto rev = m_displayWidgets.rbegin(); rev != m_displayWidgets.rend(); rev++)
    {
        if ((*rev)->contains(x, y))
        {
            return (*rev);
        }
    }
    return nullptr;
}

Size<int> Core::getCurrentWindowSize() const
{
    RECT rect;
    GetClientRect(m_windowHandle, &rect);
    return { (int)rect.right, (int)rect.bottom };
}


HRESULT Core::createGraphicsResource()
{
    HRESULT hResult = S_OK;

    if (!m_renderTarget)
    {
        Size<int> size = getCurrentWindowSize();
        hResult = m_factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_windowHandle, { (uint32_t)size.width, (uint32_t)size.height }),
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

HWND CoreInterface::getWindowHandle(Core* core)
{
    return core->m_windowHandle;
}

ID2D1Factory* CoreInterface::getFactory(Core* core)
{
    return core->m_factory;
}

IDWriteFactory* CoreInterface::getWriteFactory(Core* core)
{
    return core->m_writeFactory;
}

IWICImagingFactory* CoreInterface::getImageFactory(Core* core)
{ 
    return core->m_imageFactory;
}

void CoreInterface::addWidget(Core* core, Widget* widget)
{
    addToVector(core->m_widgets, widget);
}

void CoreInterface::removeWidget(Core* core, Widget* widget)
{
    removeFromVector(core->m_widgets, widget);
}

void CoreInterface::addDisplayWidget(Core* core, Widget* widget)
{
    addToVector(core->m_displayWidgets, widget);
}

void CoreInterface::removeDisplayWidget(Core* core, Widget* widget)
{
    removeFromVector(core->m_displayWidgets, widget);
}

void CoreInterface::addContainer(Core* core, Container* container)
{
    addToVector(core->m_containers, container);
}

void CoreInterface::removeContainer(Core* core, Container* container)
{
    removeFromVector(core->m_containers, container);
}

void CoreInterface::addTimer(Core* core, Timer* timer)
{
    addToVector(core->m_timers, timer);
}

void CoreInterface::removeTimer(Core* core, Timer* timer)
{
    removeFromVector(core->m_timers, timer);
}

void CoreInterface::setHoverWidget(Core* core, Widget* widget)
{
    core->m_hoverWidget = widget;
}

void CoreInterface::setClickWidget(Core* core, Widget* widget)
{
    core->m_clickWidget = widget;
}

void CoreInterface::setSelectWidget(Core* core, Widget* widget)
{
    core->m_selectWidget = widget;
}
