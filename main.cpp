#include <windows.h>
#include <tchar.h>
#include <d3d9.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include "src/clicker/clicker.h"
#include "src/ui/menu.h"

#pragma comment(lib, "d3d9.lib")

LPDIRECT3D9       g_pD3D       = nullptr;
LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
bool              g_visible    = false;

HWND g_MainHwnd = nullptr;

ImTextureID g_LogoTexture = nullptr;
ImVec2      g_LogoSize    = ImVec2(0.0f, 0.0f);

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    D3DPRESENT_PARAMETERS d3dpp{};
    d3dpp.Windowed         = TRUE;
    d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    if (FAILED(g_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp,
        &g_pd3dDevice)))
    {
        return false;
    }

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D)       { g_pD3D->Release(); g_pD3D = nullptr; }
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice && wParam != SIZE_MINIMIZED) { }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_MAXIMIZE) return 0;
        if ((wParam & 0xfff0) == SC_KEYMENU)  return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc,
        0L, 0L,
        GetModuleHandle(nullptr),
        nullptr, nullptr, nullptr, nullptr,
        _T("AutoClickerWnd"),
        nullptr
    };
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        wc.lpszClassName,
        _T("@nahuelzx | kdk.life"),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        100, 100, 450, 410,
        nullptr, nullptr, wc.hInstance, nullptr
    );
    g_MainHwnd = hwnd;

    if (!CreateDeviceD3D(hwnd))
    {
        MessageBox(nullptr, _T("Failed to create D3D9 device!"), _T("Error"), MB_OK);
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    ShowWindow(hwnd, SW_HIDE);
    UpdateWindow(hwnd);

    MSG  msg{};
    bool f12PressedLastFrame = false;

    while (msg.message != WM_QUIT)
    {
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        bool f12Pressed = (GetAsyncKeyState(VK_F12) & 0x8000) != 0;
        if (f12Pressed && !f12PressedLastFrame)
        {
            g_visible = !g_visible;
            ShowWindow(hwnd, g_visible ? SW_SHOW : SW_HIDE);
        }
        f12PressedLastFrame = f12Pressed;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (g_visible)
            ui::render_menu(Clicker::instance());

        ImGui::Render();

        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

        g_pd3dDevice->Clear(
            0, nullptr,
            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
            D3DCOLOR_XRGB(30, 30, 30),
            1.0f, 0
        );

        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

