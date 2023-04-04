#include "src/Dinogui.h"
#include <Windows.h>

void testClickFunc()
{
	std::cout << "Button clicked" << std::endl;
}

int main()
{
	DINOGUI::Base* base = new DINOGUI::Base("Test Window", 800, 500);
	DINOGUI::Button* button = new DINOGUI::Button(base, "Button");
	DINOGUI::Label* label = new DINOGUI::Label(base, "Label");
	DINOGUI::Checkbox* checkbox = new DINOGUI::Checkbox(base, "Checkbox");

	button->place(50, 100);
	button->connect(&testClickFunc);

	label->place(250, 100);

	checkbox->place(125, 100);

	base->run();

	return 0;
}