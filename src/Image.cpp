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
    : Widget(core), m_drawingBitmap(nullptr), m_wicBitmap(nullptr), m_imageWidth(0), m_imageHeight(0)
{
    m_type = WidgetType::IMAGE;
    ColorTheme::createDefault(m_theme, m_type);
    m_drawBorder = true;
    m_drawBackground = true;
    m_size = { 50.0f, 50.0f };
    m_maxSize = { 1e6f, 1e6f };
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

void Image::draw()
{
    D2D1_RECT_F rect = DPIHandler::adjusted(currentRect());
    basicDrawBackgroundBorder(rect);
    ID2D1HwndRenderTarget* renderTarget = getRenderTarget(m_core);
    
    if (!m_drawingBitmap && m_wicBitmap)
    {
        throwIfFailed(renderTarget->CreateBitmapFromWicBitmap(m_wicBitmap, nullptr, &m_drawingBitmap), "Failed to create Bitmap");
    }
    if (m_drawingBitmap)
    {
        D2D1_RECT_F imageRect = DPIHandler::adjusted(bitmapRect());
        renderTarget->DrawBitmap(m_drawingBitmap, rect);
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

    throwIfFailed(getImageFactory(m_core)->CreateDecoderFromFilename(toWideString(filename).c_str(), nullptr,
        GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder), "Failed to load file");
    throwIfFailed(decoder->GetFrame(0, &frameDecode), "Failed to get frame");
    throwIfFailed(getImageFactory(m_core)->CreateFormatConverter(&converter), "Failed to create converter");
    throwIfFailed(converter->Initialize(frameDecode, GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone,
        nullptr, 0.0, WICBitmapPaletteTypeCustom), "Failed to initialize converter");
    throwIfFailed(getImageFactory(m_core)->CreateBitmapFromSource(converter, WICBitmapCacheOnDemand, &m_wicBitmap));
    m_wicBitmap->GetSize(&m_imageWidth, &m_imageHeight);
    resize(m_imageWidth, m_imageHeight);

    safeReleaseInterface(&decoder);
    safeReleaseInterface(&frameDecode);
    safeReleaseInterface(&converter);
}

D2D1_RECT_F Image::bitmapRect() const
{
    D2D1_RECT_F current = currentRect();
    return { current.left + 1.0f, current.top + 1.0f, current.right, current.bottom };
}
