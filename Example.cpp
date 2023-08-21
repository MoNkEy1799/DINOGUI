#include "src/Dinogui.h"
#include "src/Random.h"

void testClickFunc(DINOGUI::Widget* wid)
{
	std::cout << "Button clicked" << std::endl;
	std::cout << ((DINOGUI::Textedit*)wid)->getText() << std::endl;
}

int main()
{
	DINOGUI::Core* core = new DINOGUI::Core("Test Window", 600, 600);
	DINOGUI::Button* button = new DINOGUI::Button(core, "Button");
	DINOGUI::Label* label = new DINOGUI::Label(core, "Label");
	DINOGUI::Checkbox* checkbox = new DINOGUI::Checkbox(core, "Checkbox");
	DINOGUI::Textedit* textedit = new DINOGUI::Textedit(core);
	DINOGUI::Image* image = new DINOGUI::Image(core, "Dino.png");
	DINOGUI::Canvas* canvas = new DINOGUI::Canvas(core);

	button->place(50, 50);
	label->place(120, 50);
	checkbox->place(190, 50);
	textedit->place(300, 50);
	image->place(50, 100);
	canvas->place(180, 100);
	Random r;
	for (int i = 0; i < 100*100; i++)
	{
		canvas->setPixel({ r.randInt(50, 255), r.randInt(50, 255) , r.randInt(50, 255) }, i);
	}

	button->connect([textedit] { testClickFunc(textedit); });

	core->run();
	return 0;
}