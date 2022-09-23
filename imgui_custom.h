/**
 * @file imgui_custom.h
 */

#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

namespace ImGui
{

static inline bool MenuItemPersistent(const std::string& label)
{
    bool ret;
    
    PushItemFlag(
        ImGuiItemFlags_SelectableDontClosePopup,
        true
    );
    
    if (MenuItem(label.c_str()))
    {
        ret = true;
    }
    else
    {
        ret = false;
    }
    
    PopItemFlag();

    return ret;
}

static inline bool Shortcut(ImGuiKeyModFlags mod, ImGuiKey key, bool repeat)
{
	return mod == GetMergedModFlags() && IsKeyPressed(GetKeyIndex(key), repeat);
}

}
