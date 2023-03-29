#include "src/Dinogui.h"
#include <Windows.h>

void testClickFunc(DINOGUI::Button* button)
{
	std::cout << "clicked" << std::endl;
}

int main()
{
	DINOGUI::Base* base = new DINOGUI::Base("Test Window", 500, 500);
	DINOGUI::Button* button = new DINOGUI::Button(base, "Button");
	DINOGUI::Label* label = new DINOGUI::Label(base, "Label");
	button->place(100, 100);
	label->place(180, 100);
	button->connect([button] { testClickFunc(button); });

	base->run();

	return 0;
}