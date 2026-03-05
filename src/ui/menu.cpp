#include "menu.h"
#include "clicker.h"
#include "imgui.h"

#include <windows.h>
#include <vector>
#include <random>

namespace ui {
    void render_menu(Clicker &clicker) {
        clicker.start_if_needed();

        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 10.0f;
        style.FrameRounding = 6.0f;
        style.WindowPadding = ImVec2(14, 14);
        style.FramePadding = ImVec2(4, 3);
        style.ItemSpacing = ImVec2(6, 4);

        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.12f, 1.0f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.12f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.12f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.26f, 0.32f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.26f, 1.0f);

        ImVec2 windowSize = ImGui::GetIO().DisplaySize;
        ImVec2 menuSize(450, 400);
        ImVec2 menuPos(
            (windowSize.x - menuSize.x) * 0.5f,
            (windowSize.y - menuSize.y) * 0.5f
        );

        ImGui::SetNextWindowSize(menuSize, ImGuiCond_Always);
        ImGui::SetNextWindowPos(menuPos, ImGuiCond_Always);

        ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoTitleBar;

        ImGui::Begin("kdk clicker", nullptr, flags);

        struct Particle {
            ImVec2 pos;
            ImVec2 vel;
        };
        static Particle particles[60];
        static bool particles_init = false;

        ImDrawList *draw = ImGui::GetWindowDrawList();
        ImVec2 winPos = ImGui::GetWindowPos();
        ImVec2 winSz = ImGui::GetWindowSize();

        if (!particles_init) {
            for (int i = 0; i < 60; ++i) {
                float x = winPos.x + (rand() % (int) winSz.x);
                float y = winPos.y + (rand() % (int) winSz.y);
                particles[i].pos = ImVec2(x, y);

                float vx = ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
                float vy = ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
                particles[i].vel = ImVec2(vx, vy);
            }
            particles_init = true;
        }

        ImU32 col = IM_COL32(150, 150, 255, 40);
        for (int i = 0; i < 60; ++i) {
            particles[i].pos.x += particles[i].vel.x;
            particles[i].pos.y += particles[i].vel.y;

            if (particles[i].pos.x < winPos.x) particles[i].pos.x = winPos.x + winSz.x;
            if (particles[i].pos.x > winPos.x + winSz.x) particles[i].pos.x = winPos.x;
            if (particles[i].pos.y < winPos.y) particles[i].pos.y = winPos.y + winSz.y;
            if (particles[i].pos.y > winPos.y + winSz.y) particles[i].pos.y = winPos.y;

            draw->AddCircleFilled(particles[i].pos, 2.0f, col, 8);
        }

        static int current_tab = 0;
        static float minCPS = 12.f;
        static float maxCPS = 16.f;
        static bool holdToClick = true;
        static bool onlyInGameWindow = false;

        ImGui::Columns(2, nullptr, true);
        ImGui::SetColumnWidth(0, menuSize.x * 0.25f);

        ImGui::SetCursorPos(ImVec2(10.0f, 15.0f));
        ImGui::SetWindowFontScale(1.8f);
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "KDK v2.0");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.20f, 1.0f, 1.0f));

        if (ImGui::Button("AutoClicker", ImVec2(-1, 40))) current_tab = 0;
        if (ImGui::Button("SelfDestruct", ImVec2(-1, 40))) current_tab = 1;
        if (ImGui::Button("Help / Binds", ImVec2(-1, 40))) current_tab = 2;

        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "discord.gg/KDK");
        ImGui::PopStyleColor(3);
        ImGui::NextColumn();

        if (current_tab == 0) {
            bool isEnabled = clicker.is_enabled();

            ImGui::Text("Status:");
            ImGui::SameLine();
            if (isEnabled)
                ImGui::TextColored(ImVec4(0.20f, 0.85f, 0.20f, 1.0f), "ON");
            else
                ImGui::TextColored(ImVec4(0.85f, 0.20f, 0.20f, 1.0f), "OFF");

            ImGui::Text("Toggle Key: F8");
            ImGui::Spacing();

            float sliderWidth = 100.0f;

            ImGui::Text("Min CPS");
            ImGui::SetNextItemWidth(sliderWidth);
            ImGui::SliderFloat("##MinCPS", &minCPS, 1.f, 25.f, "%.0f");

            ImGui::Text("Max CPS");
            ImGui::SetNextItemWidth(sliderWidth);
            ImGui::SliderFloat("##MaxCPS", &maxCPS, 1.f, 25.f, "%.0f");

            ImGui::Spacing();

            ImGui::Checkbox("Hold to click", &holdToClick);
            ImGui::Checkbox("Only in game window", &onlyInGameWindow);

            clicker.set_cps_range(minCPS, maxCPS);
            clicker.set_hold_to_click(holdToClick);
            clicker.set_only_in_game_window(onlyInGameWindow);
        } else if (current_tab == 1) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
                               "Warning: This action cannot be undone.");
            ImGui::Spacing();
            ImGui::Text("The program will erase its EXE and close.");
            ImGui::Spacing();

            if (ImGui::Button("Execute SelfDestruct", ImVec2(-1, 40))) {
                char exePath[MAX_PATH]{};
                if (GetModuleFileNameA(nullptr, exePath, MAX_PATH)) {
                    char dirPath[MAX_PATH]{};
                    strcpy_s(dirPath, exePath);
                    if (char *lastSlash = strrchr(dirPath, '\\')) {
                        *lastSlash = '\0';
                    }

                    char tempPath[MAX_PATH]{};
                    sprintf_s(tempPath, "%s\\kdk_tmp.bin", dirPath);
                    CopyFileA(exePath, tempPath, FALSE);

                    HANDLE h = CreateFileA(tempPath,
                                           GENERIC_WRITE | GENERIC_READ,
                                           0, nullptr,
                                           OPEN_EXISTING,
                                           FILE_ATTRIBUTE_NORMAL,
                                           nullptr);
                    if (h != INVALID_HANDLE_VALUE) {
                        LARGE_INTEGER size{};
                        if (GetFileSizeEx(h, &size) && size.QuadPart > 0) {
                            const int passes = 3;
                            std::vector<char> buffer(4096);
                            std::random_device rd;
                            std::mt19937 gen(rd());
                            std::uniform_int_distribution<int> dist(0, 255);

                            for (int p = 0; p < passes; ++p) {
                                SetFilePointer(h, 0, nullptr, FILE_BEGIN);
                                LONGLONG remaining = size.QuadPart;
                                while (remaining > 0) {
                                    for (auto &b: buffer)
                                        b = static_cast<char>(dist(gen));
                                    DWORD toWrite = (DWORD) std::min<LONGLONG>(buffer.size(), remaining);
                                    DWORD written = 0;
                                    WriteFile(h, buffer.data(), toWrite, &written, nullptr);
                                    if (written == 0) break;
                                    remaining -= written;
                                }
                            }
                        }
                        CloseHandle(h);
                    }

                    DeleteFileA(tempPath);

                    char args[1024]{};
                    sprintf_s(args,
                              "/c timeout /t 2 /nobreak >nul & del /f /q \"%s\"",
                              exePath);
                    ShellExecuteA(nullptr, "open", "cmd.exe", args, nullptr, SW_HIDE);
                }

                ExitProcess(0);
            }
        } else if (current_tab == 2) {
            ImGui::Text("Help & Keybinds:");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::BulletText("Toggle autoclicker: F8");
            ImGui::BulletText("Toggle menu visibility: F12");
            ImGui::BulletText("Hold to click: only clicks while LMB is held");
            ImGui::BulletText("Only in game window: only clicks when javaw.exe or Minecraft.Windows.exe is focused");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Join Discord", ImVec2(-1, 30))) {
                ShellExecuteA(
                    nullptr,
                    "open",
                    "https://discord.gg/e7J66XyDZJ",
                    nullptr,
                    nullptr,
                    SW_SHOWNORMAL
                );
            }
        }

        ImGui::Columns(1);
        ImGui::SetCursorPos(ImVec2(10, menuSize.y - 22));

        ImGui::End();
    }
}
