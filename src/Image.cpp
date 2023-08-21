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
    : Widget(core), m_drawingBitmap(nullptr), m_wicBitmap(nullptr)
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
    m_wicBitmap->GetSize(&m_imageWidth, &m_imageHeight);
    setSize(m_imageWidth, m_imageHeight);

    safeReleaseInterface(&decoder);
    safeReleaseInterface(&frameDecode);
    safeReleaseInterface(&converter);

}
