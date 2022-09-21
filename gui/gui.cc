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

extern void lapos_init();
extern std::string lapos_main(const std::string& input);

namespace Ellure
{

GUI::GUI()
{
    Doc doc{"data/testdoc"};
    word_chain = ComplexWordChain{doc.get_words()};
    filename = "test";
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
        run_editor();

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
                open_file(filename);
            }
            if (ImGui::MenuItem("Save"))
            {
                save_file();
            }
            if (ImGui::MenuItem("Close"))
            {
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

void GUI::run_editor()
{
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    bool p_open = true;
    std::string menu_action;
    
    // multiple_gen options.    
    static std::vector<std::string> options;

    // Editor window.
#ifdef IMGUI_HAS_VIEWPORT
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());
    ImGui::SetNextWindowViewport(viewport->ID);
#else 
    ImGui::SetNextWindowPos(ImVec2(0.0f, main_menu_size.y));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin("Editor", &p_open, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

    auto editor_height = ImGui::GetWindowHeight() - 4.0f * ImGui::GetTextLineHeight();
    ImGui::InputTextMultiline(
        "##editor",
        editor_text,
        IM_ARRAYSIZE(editor_text),
        ImVec2(-FLT_MIN, editor_height),
        flags
    );

    if (ImGui::BeginPopupContextItem("##editor"))
    {
        if (ImGui::MenuItem("Generate line"))
        {
            auto line = word_chain.get_line_bigrams();
            strcat(editor_text, line.c_str());
        }
        if (ImGui::MenuItem("Generate multiple line options"))
        {
            menu_action = "multiple_gen";
            options.clear();
            for (size_t i = 0; i < 5; ++i)
            {
                auto line = word_chain.get_line_bigrams();
                options.push_back(line);
            }
        }

        ImGui::EndPopup();
    }

    if (menu_action == "multiple_gen")
    {
        ImGui::OpenPopup("multiple_gen");
    }
    if (ImGui::BeginPopup("multiple_gen"))
    {
        for (size_t i = 0; i < 5; ++i)
        {
            if (ImGui::MenuItem(options[i].c_str()))
            {
                strcat(editor_text, options[i].c_str());
                options.clear();
                menu_action = "";
            }
        }

        ImGui::EndPopup();
    }

    // Finish window.
    ImGui::End();
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
    strcpy(editor_text, input.str().c_str());
    return EXIT_SUCCESS;
}

int GUI::save_file()
{
    // Add terminating newline if needed.
    const char* p = &editor_text[strlen(editor_text) - 1];
    if (*p != '\n')
    {
        strcat(editor_text, "\n");
    }
    
    std::ofstream output_file{filename};
    if (output_file << editor_text)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

}
