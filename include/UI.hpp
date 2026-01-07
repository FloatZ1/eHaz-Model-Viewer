#pragma once

#include "FileSystem.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include <Renderer.hpp>
#include <algorithm>
#include <SDL3/SDL_log.h>
#include <string>
#include <vector>

class CSelectUI {
public:
    std::vector<std::string> m_vsFiles;
    bool m_bFinished = false;
    bool m_bCanceled = false;
    std::string m_sSelectedFile;

    static bool s_bIsPreviewFocused;

    bool IsWindowContentFocused();
    void Initialize();
    void DrawGameViewPort();
    void DrawModelSelectWindow();
    void DrawButtonDock();
    std::string GetRelativeSelectedPath();
    void UpdateUI();
    void RenderUI();
};
