#include "src/Dinogui.h"
#include <Windows.h>

int main()
{
	DINOGUI::Base base = DINOGUI::Base("Test Window", 500, 500);
	DINOGUI::Button button = DINOGUI::Button(&base, "Test");
	button.place(100, 100);

	base.run();

	return 0;
}