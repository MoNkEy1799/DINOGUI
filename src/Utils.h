#pragma once

#include <d2d1.h>
#include <array>

namespace DINOGUI
{

struct Position
{
	int x, y;
};

struct Size
{
	int w, h;
};

struct Color
{
	std::array<float, 3> rgb;

	Color(float r, float g, float b) : m_d2d1Color({ r, g, b }), rgb({ r, g, b }) { };
	D2D1::ColorF getDirect2DColor() { return m_d2d1Color; };
private:
	D2D1::ColorF m_d2d1Color;
};

}
