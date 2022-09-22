/**
 * @file gui.cc
 */

#include "gui.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "complex_word_chain.h"
#include "doc.h"

// SDL
#include <SDL2/SDL.h>

// ImGui
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

// portable-file-dialogs
#include "portable-file-dialogs/portable-file-dialogs.h"

extern void lapos_init();
extern std::string lapos_main(const std::string& input);

namespace Ellure
{

static GUI* gui_singleton = nullptr;

std::string select_file_open()
{
    if (!pfd::settings::available())
    {
        std::cerr << "PFD not available on this platform.\n";
        return "";
    }
    
    auto selections = pfd::open_file("Select a file").result();
    if (!selections.empty())
    {
        return selections[0];
    }
    else
    {
        std::cerr << "User cancelled file selection.\n";
        return "";
    }
}

std::string select_file_save()
{
    if (!pfd::settings::available())
    {
        std::cerr << "PFD not available on this platform.\n";
        return "";
    }

    auto selection = pfd::save_file("Save file").result();
    if (!selection.empty())
    {
        return selection;
    }
    else
    {
        std::cerr << "User cancelled file selection.\n";
        return "";
    }
}

GUI& get_gui()
{
    while (!gui_singleton)
    {
        // Wait until GUI is created.
    }
    return *gui_singleton;
}

GUI::GUI()
{
    if (!gui_singleton)
    {
        gui_singleton = this;
    }
    filename = "";
}

int GUI::open()
{
    // SDL stub.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "[ERROR] " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    int windowWidth = 800;
    int windowHeight = 600;
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_RESIZABLE
        | SDL_WINDOW_ALLOW_HIGHDPI
    );
    sdl_window = SDL_CreateWindow(
        "Ellure",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        window_flags
    );
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1,
        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (sdl_renderer == NULL)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return EXIT_FAILURE;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(sdl_window, sdl_renderer);
    ImGui_ImplSDLRenderer_Init(sdl_renderer);
    return EXIT_SUCCESS;
}

int GUI::main_loop()
{
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                done = true;
            }
            if (event.type == SDL_WINDOWEVENT
                && event.window.event == SDL_WINDOWEVENT_CLOSE
                && event.window.windowID == SDL_GetWindowID(sdl_window))
            {
                done = true;
            }
        }

        new_frame();

        // Do stuff.
        run_main_menu();
        editor.run();

        render();
    }

    return EXIT_SUCCESS;
}

int GUI::close()
{
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}

void GUI::new_frame()
{    
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void GUI::render()
{
    ImGui::Render();
    SDL_SetRenderDrawColor(
        sdl_renderer,
        (Uint8)(clear_color.x * 255),
        (Uint8)(clear_color.y * 255),
        (Uint8)(clear_color.z * 255),
        (Uint8)(clear_color.w * 255));
    SDL_RenderClear(sdl_renderer);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(sdl_renderer);
}

void GUI::run_main_menu()
{
    if (ImGui::BeginMainMenuBar())
    {
        main_menu_size = ImGui::GetWindowSize();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
                auto user_file = select_file_open();
                if (user_file != "")
                {
                    filename = user_file;
                    open_file(filename);
                }
            }
            if (ImGui::MenuItem("Save"))
            {
                if (filename == "")
                {
                    filename = select_file_save();
                }
                save_file();
            }
            if (ImGui::MenuItem("Save as..."))
            {
                auto user_file = select_file_save();
                if (user_file != "")
                {
                    filename = user_file;
                    save_file();
                }
            }
            if (ImGui::MenuItem("Close"))
            {
                filename = "";
                strcpy(editor.text, "");
            }
            if (ImGui::MenuItem("Exit"))
            {
                close();
                exit(0);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo"))
            {
            }
            if (ImGui::MenuItem("Redo"))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut"))
            {
            }
            if (ImGui::MenuItem("Copy"))
            {
            }
            if (ImGui::MenuItem("Paste"))
            {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

int GUI::open_file(const std::string& name)
{
    std::stringstream input;
    std::ifstream input_file{name};
    if (!input_file.is_open())
    {
        std::cerr << "Error opening document.\n";
        return EXIT_FAILURE;
    }
    input << input_file.rdbuf();
    strncpy(editor.text, input.str().c_str(), sizeof(editor.text - 1));
    return EXIT_SUCCESS;
}

int GUI::save_file()
{
    // Add terminating newline if needed.
    const char* p = &editor.text[strlen(editor.text) - 1];
    if (*p != '\n')
    {
        strcat(editor.text, "\n");
    }

    if (filename == "")
    {
        return EXIT_FAILURE;
    }
    
    std::ofstream output_file{filename};
    if (output_file << editor.text)
    {
        editor.dirty = false;
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

}
