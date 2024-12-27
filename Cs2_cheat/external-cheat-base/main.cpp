#include "renderer/renderer.h"
#include "esp.h"
#include <thread>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    esp::pID = memory::GetProcID(L"cs2.exe");
    esp::modBase = memory::GetModuleBaseAddress(esp::pID, L"client.dll");




    HWND hwnd = window::InitWindow(hInstance);
    if (!hwnd) return -1;
    
    if (!renderer::init(hwnd))
    {
        renderer::destroy();
        return -1;
    }

    std::thread read(esp::loop);

    while (!GetAsyncKeyState(VK_F9)  && renderer::running)
    {
        esp::frame;
    }

    renderer::destroy();
    UnregisterClass(L"overlay", hInstance);

    return 0;


}
