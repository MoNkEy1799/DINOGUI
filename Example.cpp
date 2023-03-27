#include "src/Dinogui.h"
#include <Windows.h>

void testClickFunc()
{
	std::cout << "click" << std::endl;
}

int main()
{
	DINOGUI::Base base = DINOGUI::Base("Test Window", 500, 500);
	DINOGUI::Button button = DINOGUI::Button(&base, "Button");
	button.place(100, 100);
	button.connect(&testClickFunc);

	base.run();

	return 0;
}