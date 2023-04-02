#include "src/Dinogui.h"
#include <Windows.h>

void testClickFunc()
{
	std::cout << "Button clicked" << std::endl;
}

int main()
{
	DINOGUI::Base* base = new DINOGUI::Base("Test Window", 500, 500);
	DINOGUI::Button* button = new DINOGUI::Button(base, "Button");
	DINOGUI::Label* label = new DINOGUI::Label(base, "Label");
	DINOGUI::Checkbox* checkbox = new DINOGUI::Checkbox(base, "Checkbox");
	button->place(1, 1);
	label->place(180, 100);
	checkbox->place(210, 150);
	//label->drawBackground();
	button->connect(&testClickFunc);

	base->run();

	return 0;
}