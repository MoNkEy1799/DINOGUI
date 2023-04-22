#include "src/Dinogui.h"
#include <Windows.h>

void testClickFunc()
{
	std::cout << "Button clicked" << std::endl;
}

int main()
{
	DINOGUI::Core* core = new DINOGUI::Core("Test Window", 1000, 1000);
	DINOGUI::Button* button = new DINOGUI::Button(core, "Button");
	DINOGUI::Label* label = new DINOGUI::Label(core, "Label");
	DINOGUI::Checkbox* checkbox = new DINOGUI::Checkbox(core, "Checkbox");
	DINOGUI::Textedit* textedit = new DINOGUI::Textedit(core);
	DINOGUI::Image* image = new DINOGUI::Image(core);

	button->place(50, 100);
	label->place(250, 300);
	checkbox->place(200, 240);
	textedit->place(50, 140);
	image->place(400, 400);

	button->connect(testClickFunc);

	core->run();

	return 0;
}