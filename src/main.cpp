#include "BaseWindow.h"
#include "Button.h"

#include <Windows.h>

int main()
{
	DINOGUI::Base base = DINOGUI::Base("Test Window", 500, 500);
	DINOGUI::Button button = DINOGUI::Button(&base);
	button.place(0, 0, 0, 0);

	base.run();

	return 0;
}