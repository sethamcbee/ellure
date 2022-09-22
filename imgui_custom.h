/**
 * @file imgui_custom.h
 */

#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace ImGui
{

static bool MenuItemPersistent(const std::string& label)
{
    bool ret;
    
    ImGui::PushItemFlag(
        ImGuiItemFlags_SelectableDontClosePopup,
        true
    );
    
    if (ImGui::MenuItem("Regen"))
    {
        ret = true;
    }
    else
    {
        ret = false;
    }
    
    ImGui::PopItemFlag();

    return ret;
}

}
