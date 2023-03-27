#include "Dinogui.h"
#include "Utils.h"

#include <Windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <dwrite.h>

float DINOGUI::DPIConverter::scale = 1.0f;
D2D1_COLOR_F DINOGUI::Style::windowBackground = DINOCOLOR_MAIN_BACKGROUND;
