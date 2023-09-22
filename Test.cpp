#include "src/Dinogui.h"

void testClickFunc(DINOGUI::Widget* wid)
{
	DINOGUI::Random r;
	r.seed();
	DINOGUI::Point<float> p1 = { (float)r.randInt(0, 300), (float)r.randInt(0, 300) };
	DINOGUI::Point<float> p2 = { (float)r.randInt(0, 300), (float)r.randInt(0, 300) };
	DINOGUI::Point<float> p3 = { (float)r.randInt(0, 300), (float)r.randInt(0, 300) };
	((DINOGUI::Canvas*)wid)->fill({ 255, 255, 255 });
	((DINOGUI::Canvas*)wid)->drawTriangle(p1, p2, p3, {255, 0, 255});
	std::cout << "{ " << p1.x << ", " << p1.y << " }" << std::endl;
	std::cout << "{ " << p2.x << ", " << p2.y << " }" << std::endl;
	std::cout << "{ " << p3.x << ", " << p3.y << " }" << std::endl;
}

int main()
{
	DINOGUI::Core* core = new DINOGUI::Core("Test Window", 800, 1000);
	DINOGUI::Button* button = new DINOGUI::Button(core, "Button");
	DINOGUI::Button* button2 = new DINOGUI::Button(core, "Button2");
	DINOGUI::Button* button3 = new DINOGUI::Button(core, "Button3");
	DINOGUI::Label* label = new DINOGUI::Label(core, "Label");
	DINOGUI::Checkbox* checkbox = new DINOGUI::Checkbox(core, "Checkbox");
	DINOGUI::Textedit* textedit = new DINOGUI::Textedit(core);
	DINOGUI::Image* image = new DINOGUI::Image(core, "Dino.png");
	DINOGUI::Canvas* canvas = new DINOGUI::Canvas(core, 300, 300);
	DINOGUI::Table* table = new DINOGUI::Table(core);
	DINOGUI::Combobox* combo = new DINOGUI::Combobox(core, "Combo");
	DINOGUI::Slider* slider = new DINOGUI::Slider(core);
	DINOGUI::Slider* slider2 = new DINOGUI::Slider(core, true);

	button->place(50, 50);
	button2->place(110, 70);
	button3->place(50, 70);
	label->place(120, 50);
	checkbox->place(190, 50);
	textedit->place(300, 50);
	textedit->setPlaceholderText("Edit now");
	image->place(50, 100);
	canvas->place(180, 100);
	table->place(60, 500);
	table->setLineWidth(4.0f);
	table->resize(400, 200);
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
	//table->setCell("TEST", 1, 2, 1, 2);
	table->setColWeight(2, 1.4f);
	table->setRowWeight(1, 3.24f);
	table->setRowWeight(2, 0.46f);
	
	canvas->fill({ 255, 255, 255 });
	/*canvas->drawRectangle({-5, -5}, {40, 40}, {0, 255, 0});
	canvas->drawRectangle({ 21, 21 }, { 60, 60 }, { 255, 0, 0 });
	canvas->drawEllipse({ -20, 100 }, 90, 40, { 0, 0, 255 });
	canvas->drawCircle({ 280, 280 }, 24, { 0, 0, 0 });
	//canvas->drawLine({ 30, 250 }, { 110, 100 }, { 0, 0, 0 });*/
	//canvas->antialias(false);
	DINOGUI::Point<float> p1 = { 42, 285 };
	DINOGUI::Point<float> p2 = { 179, 123 };
	DINOGUI::Point<float> p3 = { 51, 288 };
	canvas->drawTriangle(p1, p2, p3, { 255, 0, 255 });

	combo->place(300, 20);
	combo->addItem("One");
	combo->addItem("Two");
	combo->addItem("Three");
	combo->addItem("Four");
	combo->getTextWidget(2)->setColor(DINOCOLOR_RED);

	slider->place(100, 420);
	slider2->place(90, 200);
	slider2->setMaxTicks(10);

	button->connect([canvas] { testClickFunc(canvas); });
	button2->setCheckable();

	core->run();
	return 0;
}