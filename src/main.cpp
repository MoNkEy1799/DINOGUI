#include "BaseWindow.h"

#include <Windows.h>

int main()
{
	DINOGUI::MainWindow window;

	if (!window.createWindow(L"Test Window", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(window.getWindowHandle(), SW_SHOW);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return 0;
}