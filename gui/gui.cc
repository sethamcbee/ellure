/**
 * @file gui.cc
 */

#include "gui.h"

#include <cstring>
#include <iostream>
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

void gui_editor(Ellure::ComplexWordChain& word_chain)
{
    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    static char text[1024 * 160];
    bool p_open = true;

    // Setup window.
    #ifdef IMGUI_HAS_VIEWPORT
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());
    ImGui::SetNextWindowViewport(viewport->ID);
    #else 
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    #endif
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin("Editor", &p_open, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

    auto editor_height = ImGui::GetWindowHeight() - 1.2f * ImGui::GetTextLineHeight();
    bool updated = ImGui::InputTextMultiline(
        "##editor",
        text,
        IM_ARRAYSIZE(text),
        ImVec2(-FLT_MIN, editor_height),
        flags
    );

    if (ImGui::BeginPopupContextItem("##editor"))
    {
        if (ImGui::MenuItem("Generate line"))
        {
            auto line = word_chain.get_line_bigrams();
            strcat(text, line.c_str());
        }

        ImGui::EndPopup();
    }

    // Finish window.
    ImGui::End();
    //ImGui::PopStyleVar(2);
}

int gui_main()
{
    // Ellure state.
    Ellure::Doc doc{"data/testdoc"};
    const auto& strings = doc.get_words();
    Ellure::ComplexWordChain word_chain{strings};
    
    // SDL stub.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "[ERROR] " << SDL_GetError() << std::endl;
        return -1;
    }
    int windowWidth = 800;
    int windowHeight = 600;
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_RESIZABLE
        | SDL_WINDOW_ALLOW_HIGHDPI
    );
    SDL_Window* window = SDL_CreateWindow(
        "Ellure",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        window_flags
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    // Our state.
    bool show_demo_window = false;
    bool show_another_window = false;
    bool show_editor_window = true;
    ImVec4 clear_color{0.45f, 0.55f, 0.60f, 1.00f};

    // Build output text.
    std::string output;
    for (size_t i = 0; i < 20; ++i)
    {
        output += word_chain.get_line_bigrams();
        output += "\n";
    }
    
    // Main loop.
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
                && event.window.windowID == SDL_GetWindowID(window))
            {
                done = true;
            }
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        if (show_another_window)
        {
            ImGui::Begin("Ellure");
            ImGui::Text(output.c_str());
            if (ImGui::Button("Regenerate"))
            {
                output = "";
                for (size_t i = 0; i < 20; ++i)
                {
                    output += word_chain.get_line_bigrams();
                    output += '\n';
                }
            }
            ImGui::End();
        }

        if (show_editor_window)
        {
            gui_editor(word_chain);
        }

        ImGui::Render();
        SDL_SetRenderDrawColor(
            renderer,
            (Uint8)(clear_color.x * 255),
            (Uint8)(clear_color.y * 255),
            (Uint8)(clear_color.z * 255),
            (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

}
