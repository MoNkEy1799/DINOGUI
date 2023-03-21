#include "BaseWindow.h"

#include <Windows.h>

LRESULT DINOGUI::MainWindow::HandleMessage(UINT messageCode, WPARAM wParam, LPARAM lParam)
{
    switch (messageCode)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT paintStruct;
        HBRUSH brush = CreateSolidBrush(0x0000FF00);
        HDC deviceHandle = BeginPaint(m_windowHandle, &paintStruct);
        FillRect(deviceHandle, &paintStruct.rcPaint, brush);
        EndPaint(m_windowHandle, &paintStruct);
    }
        return 0;

    default:
        return DefWindowProc(m_windowHandle, messageCode, wParam, lParam);
    }

    return 1;
}
