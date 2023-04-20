# DINOGUI 🦕
Trying to make a GUI library in C++ using Win32 API and Direct2D/ DirectWrite.
All GUI elements are implemented by myself and are not simply calls to the default Windows ClassNames (i.e. CreateWindow(L"BUTTON", ...)).

The GUI is event driven to minimize draw calls and save resources.

Currently implemented:
 - Button
 - Label
 - Checkbox
 - Textedit
