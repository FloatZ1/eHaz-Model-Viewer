#pragma once
#include "FileSystem.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include <Renderer.hpp>
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
  bool IsWindowContentFocused() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow))
      return false;

    ImVec2 mouse = ImGui::GetMousePos();
    ImVec2 min = ImGui::GetWindowContentRegionMin();
    ImVec2 max = ImGui::GetWindowContentRegionMax();
    ImVec2 pos = ImGui::GetWindowPos();

    min.x += pos.x;
    min.y += pos.y;
    max.x += pos.x;
    max.y += pos.y;

    return mouse.x >= min.x && mouse.y >= min.y && mouse.x <= max.x &&
           mouse.y <= max.y;
  }
  void Initialize() {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    s_bIsPreviewFocused = false;
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Theme (Ubuntu-like dark)
    ImGui::StyleColorsDark();

    // Adjust style for multi-viewport windows
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.WindowRounding = 5.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Initialize platforms + renderer
    ImGui_ImplSDL3_InitForOpenGL(
        eHazGraphics::Renderer::r_instance->p_window->GetWindowPtr(),
        eHazGraphics::Renderer::r_instance->p_window->GetOpenGLContext());
    ImGui_ImplOpenGL3_Init("#version 460");
  }
  void DrawGameViewPort() {
    ImGui::Begin("Viewport");

    ImVec2 size = ImGui::GetContentRegionAvail();

    int newW = std::max(1, (int)size.x);
    int newH = std::max(1, (int)size.y);

    eHazGraphics::FrameBuffer &mainFBO =
        eHazGraphics::Renderer::r_instance->GetMainFBO();

    if (mainFBO.GetWidth() != newW || mainFBO.GetHeight() != newH) {
      mainFBO.Resize(newW, newH);
    }

    ImGui::Image((void *)(uint64_t)mainFBO.GetColorTextures()[0].GetTextureID(),
                 ImVec2(mainFBO.GetWidth(), mainFBO.GetHeight()), ImVec2(0, 1),
                 ImVec2(1, 0));

    if (IsWindowContentFocused()) {
      s_bIsPreviewFocused = true;
    } else {
      s_bIsPreviewFocused = false;
    }

    ImGui::End();
  }

  void DrawModelSelectWindow() {
    ImGui::SetNextWindowDockID(ImGui::GetID("eHazDockspace"),
                               ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("file select window")) {
      ImGui::End();
      return;
    }

    ImGui::Text("File count: %d", (int)m_vsFiles.size());

    for (int i = 0; i < m_vsFiles.size(); i++) {
      bool selected = (m_sSelectedFile == m_vsFiles[i]);

      std::string display = std::filesystem::path(m_vsFiles[i]).string();

      std::string label = display + "###file_" + std::to_string(i);

      if (ImGui::Selectable(label.c_str(), selected)) {
        m_sSelectedFile = m_vsFiles[i];
        SDL_Log("%s", m_vsFiles[i].c_str());
      }
    }

    ImGui::End();
  }

  void DrawButtonDock() {

    if (!ImGui::Begin("Button Dock")) {
      ImGui::End();
      return;
    }

    if (ImGui::Button("Cancel")) {
      m_bCanceled = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Confirm")) {

      m_bFinished = true;
    }

    ImGui::End();
  }

  std::string GetRelativeSelectedPath() { return m_sSelectedFile; }

  void UpdateUI() {}

  void RenderUI() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // --- Dockspace ---
    {
      static bool dockspaceOpen = true;
      static bool opt_fullscreen = true;
      static ImGuiDockNodeFlags dockspaceFlags =
          ImGuiDockNodeFlags_PassthruCentralNode;

      ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
      if (opt_fullscreen) {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        windowFlags |= ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                       ImGuiWindowFlags_NoNavFocus;
      }

      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

      ImGui::Begin("DockSpaceWindow", &dockspaceOpen, windowFlags);
      ImGui::PopStyleVar(2);

      // Dockspace ID
      ImGuiID dockspaceID = ImGui::GetID("eHazDockspace");
      ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

      ImGui::End();
    }

    DrawModelSelectWindow();
    DrawGameViewPort();
    DrawButtonDock();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Multi-viewport windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      SDL_GLContext backup_context = SDL_GL_GetCurrentContext();
      SDL_Window *backup_window = SDL_GL_GetCurrentWindow();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      SDL_GL_MakeCurrent(backup_window, backup_context);
    }
  }
};
