#include "../src/Dinogui.h"

#include <complex>

int mandelbrot(double real, double imag, int maxIter = 150)
{
	double zReal = real;
	double zImag = imag;
	for (int i = maxIter; i > 0; i--)
	{
		double realSq = zReal * zReal;
		double imagSq = zImag * zImag;
		if (realSq + imagSq > 4.0)
		{
			return i;
		}
		zImag = 2.0 * zReal * zImag + imag;
		zReal = realSq - imagSq + real;
	}
	return 0;
}

int runMandelbrot()
{
	int scale = 300;
	DINOGUI::Core* core = new DINOGUI::Core("Mandelbrot", 900, 600);
	DINOGUI::Canvas* canvas = new DINOGUI::Canvas(core, scale * 3, scale * 2);
	canvas->place(0, 0);
	canvas->unlock();

	for (int x = 0; x < scale * 3; x++)
	{
		std::cout << "\r" << (int)(x / (scale * 3.0) * 100) << "%";
		for (int y = 0; y < scale * 2; y++)
		{
			double real = x / (double)scale - 2.0;
			double imag = y / (double)scale - 1.0;
			int iter = mandelbrot(real, imag);
			canvas->setPixel({ iter, iter, iter }, (size_t)y * scale * 3 + x, false);
		}
	}

	canvas->lock();
	core->run();

	return 0;
}

