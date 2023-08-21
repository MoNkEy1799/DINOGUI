#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <iomanip>

using namespace DINOGUI;

Canvas::Canvas(Core* core, int width, int height, Color fillColor)
    : Widget(core), m_drawingBitmap(nullptr), m_wicBitmap(nullptr), m_wicLock(nullptr),
      m_buffer(nullptr), m_bufferWidth(width), m_bufferHeight(height)
{
    m_type = WidgetType::CANVAS;
    m_drawBorder = true;
    m_size = { (float)width, (float)height };
    createPixelBuffer();
}

Canvas::~Canvas()
{
    safeReleaseInterface(&m_drawingBitmap);
    safeReleaseInterface(&m_wicBitmap);
    safeReleaseInterface(&m_wicLock);
}

void Canvas::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    drawBasicShape(renderTarget, brush);

    if (!m_drawingBitmap && m_wicBitmap)
    {
        throwIfFailed(renderTarget->CreateBitmapFromWicBitmap(m_wicBitmap, nullptr, &m_drawingBitmap), "Failed to create Bitmap");
    }

    if (m_drawingBitmap)
    {
        renderTarget->DrawBitmap(m_drawingBitmap, currentRect());
    }
}

void Canvas::place(int x, int y)
{
    basicPlace(x, y);
}

void Canvas::createPixelBuffer()
{
    uint32_t* buffer = new uint32_t[m_bufferWidth * m_bufferHeight]{ 0 };
    throwIfFailed(m_core->getImageFactory()->CreateBitmapFromMemory(m_bufferWidth, m_bufferHeight, GUID_WICPixelFormat32bppPRGBA,
        m_bufferWidth * 4, m_bufferWidth * m_bufferHeight * 4, (byte*)buffer, &m_wicBitmap));
    setSize(m_bufferWidth, m_bufferHeight);
    delete[] buffer;
}

void Canvas::fill(Color color, bool autoLock)
{
    if (autoLock)
    {
        unlock();
    }
    else if (!m_buffer)
    {
        return;
    }

    for (int pos = 0; pos < m_bufferWidth * m_bufferHeight; pos++)
    {
        m_buffer[pos * 4 + 0] = (byte)color.r;
        m_buffer[pos * 4 + 1] = (byte)color.g;
        m_buffer[pos * 4 + 2] = (byte)color.b;
        m_buffer[pos * 4 + 3] = (byte)color.a;
    }

    if (autoLock)
    {
        lock();
    }
}

void Canvas::setPixel(Color color, size_t pos, bool autoLock)
{
    if (autoLock)
    {
        unlock();
    }
    else if (!m_buffer)
    {
        return;
    }

    m_buffer[pos * 4 + 0] = (byte)color.r;
    m_buffer[pos * 4 + 1] = (byte)color.g;
    m_buffer[pos * 4 + 2] = (byte)color.b;
    m_buffer[pos * 4 + 3] = (byte)color.a;

    if (autoLock)
    {
        lock();
    }
}

void Canvas::lock()
{
    m_buffer = nullptr;
    safeReleaseInterface(&m_wicLock);
}

void Canvas::unlock()
{
    WICRect wicRect = { 0, 0, m_bufferWidth, m_bufferHeight };
    uint32_t bufferSize;
    m_wicBitmap->Lock(&wicRect, WICBitmapLockWrite, &m_wicLock);
    m_wicLock->GetDataPointer(&bufferSize, &m_buffer);
}
