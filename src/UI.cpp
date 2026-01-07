#include "glm/trigonometric.hpp"
#include "imgui.h"
#include <UI.hpp>
#include <glm/gtc/type_ptr.hpp>

bool CSelectUI::s_bIsPreviewFocused = false;

bool CSelectUI::IsWindowContentFocused() {
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
void CSelectUI::Initialize() {

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  s_bIsPreviewFocused = false;
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  // Theme (Ubuntu-like dark)
  ImVec4 *colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.29f, 0.34f, 0.26f, 1.00f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.29f, 0.34f, 0.26f, 1.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
  colors[ImGuiCol_Border] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.14f, 0.16f, 0.11f, 0.52f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.27f, 0.30f, 0.23f, 1.00f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.34f, 0.26f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.34f, 0.26f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.32f, 0.24f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.30f, 0.22f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.23f, 0.27f, 0.21f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
  colors[ImGuiCol_Button] = ImVec4(0.29f, 0.34f, 0.26f, 0.40f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
  colors[ImGuiCol_Header] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.42f, 0.31f, 0.6f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.54f, 0.57f, 0.51f, 0.50f);
  colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.16f, 0.11f, 1.00f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.54f, 0.57f, 0.51f, 1.00f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_ResizeGrip] =
      ImVec4(0.19f, 0.23f, 0.18f, 0.00f); // grip invis
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.54f, 0.57f, 0.51f, 1.00f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_Tab] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.54f, 0.57f, 0.51f, 0.78f);
  colors[ImGuiCol_TabActive] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.24f, 0.27f, 0.20f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.35f, 0.42f, 0.31f, 1.00f);
  colors[ImGuiCol_DockingPreview] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.78f, 0.28f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.73f, 0.67f, 0.24f, 1.00f);
  colors[ImGuiCol_NavHighlight] = ImVec4(0.59f, 0.54f, 0.18f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

  ImGuiStyle &style = ImGui::GetStyle();
  style.FrameBorderSize = 1.0f;
  style.WindowRounding = 0.0f;
  style.ChildRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.PopupRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 0.0f;

  // Initialize platforms + renderer
  ImGui_ImplSDL3_InitForOpenGL(
      eHazGraphics::Renderer::r_instance->p_window->GetWindowPtr(),
      eHazGraphics::Renderer::r_instance->p_window->GetOpenGLContext());
  ImGui_ImplOpenGL3_Init("#version 460");
}
void CSelectUI::DrawGameViewPort() {
  ImGui::Begin("Viewport");

  ImVec2 size = ImGui::GetContentRegionAvail();

#ifdef PLATFORM_LINUX
  int newW = std::max(1, (int)size.x);
  int newH = std::max(1, (int)size.y);
#elif defined(PLATFORM_WINDOWS)
  int newW = max(1, (int)size.x);
  int newH = max(1, (int)size.y);
#endif
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

void CSelectUI::DrawModelSelectWindow() {
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

void CSelectUI::DrawButtonDock() {

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

  ImGui::Separator();

  if (ImGui::Button("Reset")) {
    m_vec3EulerDegrees = glm::vec3(0.0f);

    m_vec3Position = glm::vec3(0.0f);
    m_vec3Scale = glm::vec3(1.0f);
    m_fScale = 1.0f;
  }

  ImGui::DragFloat3("Position", glm::value_ptr(m_vec3Position), 0.01f);
  ImGui::DragFloat("Scale", &m_fScale, 0.01f, 0.01f, 100.0f);

  m_vec3Scale = glm::vec3(m_fScale);

  if (ImGui::DragFloat3("Rotation", glm::value_ptr(m_vec3EulerDegrees), 1.0f)) {
    m_quatRotation = glm::quat(glm::radians(m_vec3EulerDegrees));
  }

  ImGui::End();
}

glm::vec3 CSelectUI::GetPositionModifiers() { return m_vec3Position; }
glm::vec3 CSelectUI::GetScaleModifiers() { return m_vec3Scale; }
glm::quat CSelectUI::GetRotationModifiers() { return m_quatRotation; }
std::string CSelectUI::GetRelativeSelectedPath() { return m_sSelectedFile; }

void CSelectUI::UpdateUI() {}

void CSelectUI::RenderUI() {

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
      windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
      windowFlags |=
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
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
