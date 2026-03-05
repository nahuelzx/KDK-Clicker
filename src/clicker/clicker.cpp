#include "clicker.h"
#include <windows.h>
#include <psapi.h>
#include <random>
#include <chrono>

using namespace std::chrono_literals;

extern HWND g_MainHwnd;

Clicker& Clicker::instance() {
    static Clicker inst;
    return inst;
}

Clicker::Clicker() {
}

void Clicker::set_cps_range(float min_val, float max_val) {
    min_cps = min_val;
    max_cps = max_val;
    clamp_cps();
}

void Clicker::set_hold_to_click(bool value) {
    hold_to_click = value;
}

void Clicker::set_only_in_game_window(bool value) {
    only_in_game_window = value;
}

void Clicker::start_if_needed() {
    bool expected = false;
    if (threads_started.compare_exchange_strong(expected, true)) {
        CreateThread(nullptr, 0, mouse_logger, nullptr, 0, nullptr);
        std::thread(&Clicker::clicker_loop, this).detach();
        std::thread(&Clicker::keybind_watcher_loop, this).detach();
    }
}

void Clicker::clamp_cps() {
    if (min_cps < 1.f)  min_cps = 1.f;
    if (max_cps < 1.f)  max_cps = 1.f;
    if (min_cps > 25.f) min_cps = 25.f;
    if (max_cps > 25.f) max_cps = 25.f;
    if (min_cps > max_cps) std::swap(min_cps, max_cps);
}

void Clicker::perform_click() {
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dx = 0;
    input.mi.dy = 0;
    input.mi.mouseData = 0;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    input.mi.dwExtraInfo = 0;
    input.mi.time = 0;
    SendInput(1, &input, sizeof(INPUT));

    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

LRESULT CALLBACK Clicker::low_level_mouse_proc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        if (info && info->flags != LLMHF_INJECTED) {
            if (wParam == WM_LBUTTONDOWN)
                Clicker::instance().mouse_down.store(true);
            if (wParam == WM_LBUTTONUP)
                Clicker::instance().mouse_down.store(false);
        }
    }
    return CallNextHookEx(Clicker::instance().h_mouse_hook, nCode, wParam, lParam);
}

DWORD WINAPI Clicker::mouse_logger(LPVOID) {
    Clicker& c = Clicker::instance();
    c.h_mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, low_level_mouse_proc, nullptr, 0);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        Sleep(1);
    }

    if (c.h_mouse_hook)
        UnhookWindowsHookEx(c.h_mouse_hook);

    return 0;
}

void Clicker::keybind_watcher_loop() {
    while (true) {
        if (!capturing_key.load()) {
            if (GetAsyncKeyState(static_cast<int>(keybind.load())) & 1) {
                enabled.store(!enabled.load());
            }
        }
        std::this_thread::sleep_for(10ms);
    }
}

void Clicker::clicker_loop() {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> jitter(0.9f, 1.1f);

    while (true) {
        if (!enabled.load()) {
            std::this_thread::sleep_for(5ms);
            continue;
        }

        if (only_in_game_window) {
            if (g_MainHwnd != nullptr) {
                POINT p;
                if (GetCursorPos(&p)) {
                    RECT rc;
                    if (GetWindowRect(g_MainHwnd, &rc)) {
                        bool overMenu =
                            p.x >= rc.left  && p.x <= rc.right &&
                            p.y >= rc.top   && p.y <= rc.bottom;

                        if (overMenu) {
                            std::this_thread::sleep_for(5ms);
                            continue;
                        }
                    }
                }
            }
        }

        bool canClick = true;
        if (hold_to_click)
            canClick = mouse_down.load();

        if (!canClick) {
            std::this_thread::sleep_for(1ms);
            continue;
        }

        clamp_cps();
        std::uniform_int_distribution<int> cpsDist(
            static_cast<int>(min_cps),
            static_cast<int>(max_cps)
        );
        int cps = cpsDist(gen);

        perform_click();
        int delay = int((1000.0f / cps) * jitter(gen));
        if (delay < 1) delay = 1;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

