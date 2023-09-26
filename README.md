# DINOGUI 🦕
A simple to use GUI toolkit in C++ using the Win32 API and Direct2D/ DirectWrite.
All GUI elements are implemented from scratch and are not simply calls to the default Windows ClassNames (i.e. CreateWindow(L"BUTTON", ...)). The GUI is event driven to minimize draw calls and save resources. All widgets can (and should) be created on the heap and are automatically deleted with distruction of the parent window. A Python version that accesses a .dll is also planned.

It is heavily inspired by Qt & Tkinter whith a focus on simplicity (Tkinter) and quality of life (Qt) for fast and simple GUI based programs.

## Quickstart

To get started, first create a `DINOGUI::Core` which is the base window & main application for all other widgets and windows. Then, create other widgets (preferably on the heap) and `place` or `grid` them in the window. Lastly start the event loop by calling the `run` methode. Afterwards delete the core and therefore automatically clean up all other widgets (or stack-allocate the core).

```cpp
#include "Dinogui.h"

int main()
{
    DINOGUI::Core* core = new DINOGUI::Core("Example Window", 400, 400);
    DINOGUI::Button* button = new DINOGUI::Button(core, "My Button");
    
    button->place(200, 200);
    
    core->run();
    delete core;
    
    return 0;
}
```

## Widgets

Currently implemented/ planned:
 - Button
 - Label
 - Checkbox
 - Textedit (unfinished)
 - Image
 - Canvas
 - Table
 - Combobox
 - Slider
 - Toplevel Window (planned)
