#include "src/Dinogui.h"

void testClickFunc(DINOGUI::Core* c)
{
	
}

void func(DINOGUI::Widget* wid, std::array<DINOGUI::Point<float>, 3>& points, bool& a)
{
	a = !a;
	((DINOGUI::Canvas*)wid)->antialias(a);
	((DINOGUI::Canvas*)wid)->fill({ 255, 255, 255 });
	((DINOGUI::Canvas*)wid)->drawTriangle(points[0], points[1], points[2], { 0, 0, 255 });
}

int main()
{
	DINOGUI::Core* core = new DINOGUI::Core("Test Window", 200, 200);
	//DINOGUI::Button* button = new DINOGUI::Button(core, "Button", [core] { testClickFunc(core); });
	/*DINOGUI::Button* button2 = new DINOGUI::Button(core, "Button2");
	DINOGUI::Button* button3 = new DINOGUI::Button(core, "Button3");
	DINOGUI::Label* label = new DINOGUI::Label(core, "Label");
	DINOGUI::Checkbox* checkbox = new DINOGUI::Checkbox(core, "Checkbox");*/
	DINOGUI::Textedit* textedit = new DINOGUI::Textedit(core);
	/*DINOGUI::Image* image = new DINOGUI::Image(core, "Dino.png");
	DINOGUI::Canvas* canvas = new DINOGUI::Canvas(core, 300, 300);
	DINOGUI::Table* table = new DINOGUI::Table(core);
	DINOGUI::Combobox* combo = new DINOGUI::Combobox(core, "Combo");
	DINOGUI::Slider* slider = new DINOGUI::Slider(core);
	DINOGUI::Slider* slider2 = new DINOGUI::Slider(core, true);*/

	//button->place(50, 50);
	/*button2->place(110, 70);
	button3->place(50, 70);
	label->place(120, 50);
	checkbox->place(190, 50);*/
	textedit->place(50, 50);
	textedit->setPlaceholderText("Edit now");
	/*image->place(50, 100);
	canvas->place(180, 100);
	table->place(60, 500);
	table->setLineWidth(4.0f);
	table->setCell("One", 3, 0, 1, 2);
	table->setCell("Two", 3, 2, 1, 2);
	table->setCell("Three", 3, 4, 1, 2);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			table->setCell(std::to_string(j), i, j);
		}
	}
	canvas->fill({ 255, 255, 255 });
	canvas->drawRectangle({-5, -5}, {40, 40}, {0, 255, 0});
	canvas->drawRectangle({ 21, 21 }, { 60, 60 }, { 255, 0, 0 });
	canvas->drawEllipse({ -20, 100 }, 90, 40, { 0, 0, 255 });
	canvas->drawCircle({ 280, 280 }, 44, { 0, 0, 0 });
	canvas->drawLine({ 40, 250 }, { 180, 270 }, { 200, 200, 0 });
	canvas->drawTriangle({ 210, 30 }, { 140, 230 }, { 270, 260 }, { 255, 50, 100, 150 });

	combo->place(300, 20);
	combo->addItem("One");
	combo->addItem("Two");
	combo->addItem("Three");
	combo->addItem("Four");
	combo->getTextWidget(2)->setColor(DINOCOLOR_RED);

	slider->place(100, 420);
	slider2->place(90, 200);
	slider2->setMaxTicks(10);

	std::array<DINOGUI::Point<float>, 3> points = { 0 };
	bool antialias = true;
	button->connect([canvas, &points]() { testClickFunc(canvas, points); });
	button2->setCheckable();
	button3->connect([canvas, &points, &antialias]() { func(canvas, points, antialias); });*/

	core->run();
	return 0;
}