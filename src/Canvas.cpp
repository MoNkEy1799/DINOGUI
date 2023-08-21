#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cmath>

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

void Canvas::drawLine(int xa, int ya, int xb, int yb, Color color, bool autoLock)
{
    if (autoLock)
    {
        unlock();
    }
    else if (!m_buffer)
    {
        return;
    }



    if (autoLock)
    {
        lock();
    }
}

void Canvas::drawRectangle(int xa, int ya, int xb, int yb, Color color, bool autoLock)
{
    if (autoLock)
    {
        unlock();
    }
    else if (!m_buffer)
    {
        return;
    }

    int xmin, xmax, ymin, ymax;
    if (xa < xb)
    {
        xmin = xa;
        xmax = xb;
    }
    else
    {
        xmin = xb;
        xmax = xa;
    }
    if (ya < yb)
    {
        ymin = ya;
        ymax = yb;
    }
    else
    {
        ymin = yb;
        ymax = ya;
    }
    checkBounds(xmin);
    checkBounds(xmax);
    checkBounds(ymin);
    checkBounds(ymax);

    for (int x = xmin; x < xmax + 1; x++)
    {
        for (int y = ymin; y < ymax + 1; y++)
        {
            m_buffer[(y * m_bufferWidth + x) * 4 + 0] = (byte)color.r;
            m_buffer[(y * m_bufferWidth + x) * 4 + 1] = (byte)color.g;
            m_buffer[(y * m_bufferWidth + x) * 4 + 2] = (byte)color.b;
            m_buffer[(y * m_bufferWidth + x) * 4 + 3] = (byte)color.a;
        }
    }

    if (autoLock)
    {
        lock();
    }
}

void Canvas::drawCircle(int x, int y, int r, Color color, bool autoLock)
{
    drawEllipse(x, y, r, r, color, autoLock);
}

void Canvas::drawEllipse(int x, int y, int ra, int rb, Color color, bool autoLock)
{
    if (autoLock)
    {
        unlock();
    }
    else if (!m_buffer)
    {
        return;
    }

    int xmin = x - ra;
    int xmax = x + ra;
    int ymin = y - rb;
    int ymax = y + rb;
    checkBounds(xmin);
    checkBounds(xmax);
    checkBounds(ymin);
    checkBounds(ymax);

    for (int xs = xmin; xs < xmax + 1; xs++)
    {
        for (int ys = ymin; ys < ymax + 1; ys++)
        {
            float ellipse = std::pow(xs - x, 2) / std::pow(ra, 2) + std::pow(ys - y, 2) / std::pow(rb, 2);
            if (ellipse <= 1)
            {
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 0] = (byte)color.r;
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 1] = (byte)color.g;
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 2] = (byte)color.b;
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 3] = (byte)color.a;
            }
            else if (ellipse < 1 + 1.0f / ra + 1.0f / rb)
            {
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 0] += (byte)color.r;
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 0] /= 2;
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 1] += (byte)color.g;
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 1] /= 2;
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 2] += (byte)color.b;
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 2] /= 2;
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 3] += (byte)color.a;
                m_buffer[(ys * m_bufferWidth + xs) * 4 + 3] /= 2;
            }
        }
    }

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

void Canvas::checkBounds(int& n)
{
    if (n < 0)
    {
        n = 0;
    }
    else if (n > m_bufferWidth)
    {
        n = m_bufferWidth;
    }
}
