#include "../src/Dinogui.h"

#include <complex>

int iteration(std::complex<double> c)
{
	std::complex<double> start(0.0, 0.0);
	for (int i = 10; i > 0; i--)
	{
		start += std::pow(start, 2) + c;
		if (std::abs(start) > 2)
		{
			return i;
		}
	}
	return 0;
}

int main()
{
	int scale = 400;
	DINOGUI::Core* core = new DINOGUI::Core("Mandelbrot", scale * 3, scale * 2);
	DINOGUI::Canvas* canvas = new DINOGUI::Canvas(core, scale * 3, scale * 2);
	canvas->place(0, 0);
	canvas->unlock();

	for (int real = 0; real < scale * 3; real++)
	{
		std::cout << "\r" << (int)(real / (scale * 3.0) * 100) << "%";
		for (int imag = 0; imag < scale * 2; imag++)
		{
			int iter = iteration(std::complex<double>(real / (double)scale - 2.0, imag / (double)scale - 1.0));
			canvas->setPixel({ iter, iter, iter }, imag * scale * 3 + real, false);
		}
	}

	canvas->lock();
	core->run();
}

