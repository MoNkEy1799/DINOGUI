# DINOGUI 🦕
Trying to make a GUI library in C++ using Win32 API and Direct2D/ DirectWrite.
All GUI elements are implemented by myself and are not simply calls to the default Windows ClassNames (i.e. CreateWindow(L"BUTTON", ...)).

The GUI is event driven to minimize draw calls and save resources. A Python version is that accesses a .dll is also planned.

## Quickstart

To get started, first create a `DINOGUI::Core` widget which is the base window for all other widgets. Then create other widgets on the heap and `place` them (memory is automatically freed upon deletion of the window). Lastly start the event loop by calling the `run` methode.

```cpp
#include <Dinogui.h>

int main()
{
    DINOGUI::Core* core = new DINOGUI::Core("Example Window", 400, 400);
    DINOGUI::Button* button = new DINOGUI::Button(core, "My Button");
    
    button->place(200, 200);
    
    core->run();
    
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
 - Combobox (unfinished)
 - Slider (planned)
 - Toplevel Window (planned)
