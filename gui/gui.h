/**
 * @file gui.h
 */

#pragma once

#include <string>

#include "imgui/imgui.h"

#include "complex_word_chain.h"
#include "gui/editor.h"

struct SDL_Window;
struct SDL_Renderer;

namespace Ellure
{

class GUI;

GUI& get_gui();

class GUI
{
public:

    GUI();

    // Main GUI operations.
    int open();
    int main_loop();
    int close();

    // Rendering functions.
    void new_frame();
    void render();

    void run_main_menu();
    void run_global_shortcuts();

    // User actions.
    void prompt_new_file();
    void prompt_open_file();
    void prompt_save_file();
    void prompt_save_file_as();
    void prompt_quit();

    // File operations.
    int open_file(const std::string& name);
    int save_file();

    // Public data.
    ImVec2 main_menu_size;

private:

    // Renderer.
    SDL_Window* sdl_window = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;

    // IMGUI config and data.
    ImVec4 clear_color{0.45f, 0.55f, 0.60f, 1.00f};
    bool save_then_new = false;
    bool save_then_open = false;
    bool save_then_quit = false;

    // Runtime data.
    Editor editor;
    std::string filename;
};

}
