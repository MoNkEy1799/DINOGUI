#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <stdexcept>

using namespace DINOGUI;

Image::Image(Core* core)
    : Widget(core)
{
    m_type = WidgetType::IMAGE;
    m_drawBorder = true;
    m_size = { 200.0f, 200.0f };
}

Image::~Image()
{

}

void Image::draw(ID2D1HwndRenderTarget* renderTarget, ID2D1SolidColorBrush* brush)
{
    drawBasicShape(renderTarget, brush);

    throwIfFailed(m_core->getImageFactory()->CreateDecoderFromFilename(
        L"Dino.png",
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,
        &m_decoder
    ), "Failed to create Image Decoder");

    IWICBitmapFrameDecode* frameDecode;
    throwIfFailed(m_decoder->GetFrame(0, &frameDecode), "Failed to get frame");
    IWICFormatConverter* converter;
    throwIfFailed(m_core->getImageFactory()->CreateFormatConverter(&converter), "Failed to create converter");
    throwIfFailed(converter->Initialize(
        frameDecode,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    ), "Failed to initialize converter");
    ID2D1Bitmap* bitmap;
    throwIfFailed(renderTarget->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap), "Failed to create Bitmap");
    renderTarget->DrawBitmap(bitmap, currentRect());

}

void Image::place(int x, int y)
{
    basicPlace(x, y);
}