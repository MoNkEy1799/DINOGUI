#pragma once

#include <Windows.h>

namespace DINOGUI
{

template<class C>
void safeReleaseInterface(C** pointerToInterface)
{
	if (*pointerToInterface)
	{
		(*pointerToInterface)->Release();
		*pointerToInterface = nullptr;
	}
}

template<class CLASS_TYPE>
class TemplateWindow
{
public:
	TemplateWindow() : m_windowHandle(nullptr) { }

	static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT messageCode, WPARAM wParam, LPARAM lParam)
	{
		CLASS_TYPE* mainThis = nullptr;

		if (messageCode == WM_NCCREATE)
		{
			CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
			mainThis = (CLASS_TYPE*)createStruct->lpCreateParams;
			SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR)mainThis);

			mainThis->m_windowHandle = windowHandle;
		}

		else
		{
			mainThis = (CLASS_TYPE*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);
		}

		if (mainThis)
		{
			return mainThis->HandleMessage(messageCode, wParam, lParam);
		}

		else
		{
			return DefWindowProc(windowHandle, messageCode, wParam, lParam);
		}
	}

	bool createWindow(PCWSTR windowName, DWORD windowStyle, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT)
	{
		WNDCLASS windowClass = { 0 };

		windowClass.lpfnWndProc = CLASS_TYPE::WindowProc;
		windowClass.hInstance = GetModuleHandle(0);
		windowClass.lpszClassName = L"BaseWindow";
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

		RegisterClass(&windowClass);

		m_windowHandle = CreateWindowEx(0, L"BaseWindow", windowName, windowStyle, x, y, width, height, 0, 0, GetModuleHandle(0), this);

		return (m_windowHandle ? true : false);
	}

	HWND getWindowHandle() { return m_windowHandle; };

protected:
	virtual LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam) = 0;

	HWND m_windowHandle;
};

}
