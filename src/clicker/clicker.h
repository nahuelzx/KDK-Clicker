#pragma once
#include <Windows.h>
#include <atomic>
#include <thread>
#include <random>
#include <chrono>

class Clicker {
public:
    static Clicker& instance();

    void start_if_needed();
    bool is_enabled() const { return enabled.load(); }

    void set_cps_range(float min_cps, float max_cps);
    void set_hold_to_click(bool value);
    void set_only_in_game_window(bool value);

private:
    Clicker();
    ~Clicker() = default;
    Clicker(const Clicker&) = delete;
    Clicker& operator=(const Clicker&) = delete;

    void clicker_loop();
    void keybind_watcher_loop();

    static LRESULT CALLBACK low_level_mouse_proc(int nCode, WPARAM wParam, LPARAM lParam);
    static DWORD WINAPI mouse_logger(LPVOID);
    void perform_click();
    void clamp_cps();

private:
    std::atomic<bool> enabled{ false };
    std::atomic<UINT> keybind{ VK_F8 };

    float min_cps = 12.0f;
    float max_cps = 16.0f;

    bool hold_to_click = true;
    bool only_in_game_window = false;

    std::atomic<bool> mouse_down{ false };
    HHOOK h_mouse_hook{};
    std::atomic<bool> threads_started{ false };
    std::atomic<bool> capturing_key{ false };
};
