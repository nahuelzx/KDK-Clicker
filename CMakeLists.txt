cmake_minimum_required(VERSION 3.26)
project(kdkclicker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -s")
add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)

set(SOURCES
        main.cpp
        src/clicker/clicker.cpp
        src/ui/menu.cpp
)

set(IMGUI_SOURCES
        src/Lib/imgui/imgui.cpp
        src/Lib/imgui/imgui_draw.cpp
        src/Lib/imgui/imgui_tables.cpp
        src/Lib/imgui/imgui_widgets.cpp
        src/Lib/imgui/imgui_impl_win32.cpp
        src/Lib/imgui/imgui_impl_dx9.cpp
)

add_executable(${PROJECT_NAME} WIN32
        ${SOURCES}
        ${IMGUI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
        src
        src/clicker
        src/ui
        src/Lib/imgui
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d9
        dwmapi
        user32
        gdi32
        d3dx9
)
