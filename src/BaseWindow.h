#pragma once

#include <Windows.h>

namespace DINOGUI
{
	template<class CLASS_TYPE>
	class BaseWindow
	{
	public:
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

		BaseWindow() : m_windowHandle(nullptr) { }

		bool createWindow(PCWSTR windowName, DWORD windowStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT)
		{
			WNDCLASS windowClass = { 0 };

			windowClass.lpfnWndProc = CLASS_TYPE::WindowProc;
			windowClass.hInstance = GetModuleHandle(0);
			windowClass.lpszClassName = L"BaseWindow";

			RegisterClass(&windowClass);

			m_windowHandle = CreateWindowEx(0, L"BaseWindow", windowName, windowStyle, x, y, width, height, 0, 0, GetModuleHandle(0), this);

			return (m_windowHandle ? true : false);
		}

		HWND getWindowHandle() const { return m_windowHandle; };

	protected:
		virtual LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam) = 0;

		HWND m_windowHandle;
	};

	class MainWindow : public BaseWindow<MainWindow>
	{
	public:
		LRESULT HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam);
	};
}
