#include "BaseWindow.h"
#include "Button.h"

#include <Windows.h>

int main()
{
	DINOGUI::Base base = DINOGUI::Base();
	DINOGUI::Button button = DINOGUI::Button(base);

	base.run();

	return 0;
}