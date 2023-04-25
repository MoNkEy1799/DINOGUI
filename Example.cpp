#include "src/Dinogui.h"
#include <Windows.h>

void testClickFunc(DINOGUI::Image* image)
{
	std::cout << "Button clicked" << std::endl;
	image->show();
}

int main()
{
	DINOGUI::Core* core = new DINOGUI::Core("Test Window", 600, 600);
	DINOGUI::Button* button = new DINOGUI::Button(core, "Button");
	DINOGUI::Label* label = new DINOGUI::Label(core, "Label");
	DINOGUI::Checkbox* checkbox = new DINOGUI::Checkbox(core, "Checkbox");
	DINOGUI::Textedit* textedit = new DINOGUI::Textedit(core);
	DINOGUI::Image* image = new DINOGUI::Image(core);// , "Dino.png");

	button->place(50, 50);
	label->place(120, 50);
	checkbox->place(190, 50);
	textedit->place(300, 50);
	image->place(100, 100);
	image->loadPixelData();
	image->setSize(128, 12);

	button->connect([image] {testClickFunc(image); });

	core->run();

	return 0;
}