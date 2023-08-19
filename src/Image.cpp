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

Image::Image(Core* core, const std::string& filename)
    : Widget(core), m_drawingBitmap(nullptr), m_wicBitmap(nullptr), m_wicLock(nullptr),
      m_buffer(nullptr), m_bufferWidth(0), m_bufferHeight(0)
{
    m_type = WidgetType::IMAGE;
    m_drawBorder = true;
    m_size = { 200.0f, 200.0f };
    if (!filename.empty())
    {
        loadImageFromFile(filename);
    }
}

Image::~Image()
{
    safeReleaseInterface(&m_drawingBitmap);
    safeReleaseInterface(&m_wicBitmap);
    safeReleaseInterface(&m_wicLock);
}

void Image::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
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

void Image::place(int x, int y)
{
    basicPlace(x, y);
}

void Image::loadImageFromFile(const std::string& filename)
{
    safeReleaseInterface(&m_wicBitmap);
    safeReleaseInterface(&m_drawingBitmap);

    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frameDecode = nullptr;
    IWICFormatConverter* converter = nullptr;

    throwIfFailed(m_core->getImageFactory()->CreateDecoderFromFilename(toWideString(filename).c_str(), nullptr,
        GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder), "Failed to load file");
    throwIfFailed(decoder->GetFrame(0, &frameDecode), "Failed to get frame");
    throwIfFailed(m_core->getImageFactory()->CreateFormatConverter(&converter), "Failed to create converter");
    throwIfFailed(converter->Initialize(frameDecode, GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone,
        nullptr, 0.0, WICBitmapPaletteTypeCustom), "Failed to initialize converter");
    throwIfFailed(m_core->getImageFactory()->CreateBitmapFromSource(converter, WICBitmapCacheOnDemand, &m_wicBitmap));

    safeReleaseInterface(&decoder);
    safeReleaseInterface(&frameDecode);
    safeReleaseInterface(&converter);
}

void DINOGUI::Image::createPixelBuffer(int width, int height)
{
    m_bufferWidth = width;
    m_bufferHeight = height;
    uint32_t* buffer = new uint32_t[width * height] { 0 };
    throwIfFailed(m_core->getImageFactory()->CreateBitmapFromMemory(width, height, GUID_WICPixelFormat32bppPRGBA,
        width * 4, width * height * 4, (byte*)buffer, &m_wicBitmap));
    setSize(width, height);
    delete[] buffer;
}

void DINOGUI::Image::fillBuffer(Color color)
{
    if (!m_buffer)
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
}

void DINOGUI::Image::setPixel(Color color, size_t pos)
{
    if (!m_buffer)
    {
        return;
    }

    m_buffer[pos * 4 + 0] = (byte)color.r;
    m_buffer[pos * 4 + 1] = (byte)color.g;
    m_buffer[pos * 4 + 2] = (byte)color.b;
    m_buffer[pos * 4 + 3] = (byte)color.a;
}

void DINOGUI::Image::lockBuffer()
{
    m_buffer = nullptr;
    safeReleaseInterface(&m_wicLock);
}

void DINOGUI::Image::unlockBuffer()
{
    WICRect wicRect = { 0, 0, m_bufferWidth, m_bufferHeight };
    uint32_t bufferSize;
    m_wicBitmap->Lock(&wicRect, WICBitmapLockWrite, &m_wicLock);
    m_wicLock->GetDataPointer(&bufferSize, &m_buffer);
}
