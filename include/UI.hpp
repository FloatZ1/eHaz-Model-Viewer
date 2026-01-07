#pragma once

#include "FileSystem.hpp"
#include "glm/fwd.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include <Renderer.hpp>
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <string>
#include <vector>

class CSelectUI {
public:
  std::vector<std::string> m_vsFiles;
  bool m_bFinished = false;
  bool m_bCanceled = false;
  std::string m_sSelectedFile;

  glm::vec3 m_vec3Position = glm::vec3(0.0f);
  glm::vec3 m_vec3Scale = glm::vec3(1.0f);
  glm::quat m_quatRotation = glm::quat(glm::radians(m_vec3Position));
  glm::vec3 m_vec3EulerDegrees = {0.0f, 0.0f, 0.0f};
  float m_fScale = 1.0f;
  float m_fRotation = 0.0f;

  static bool s_bIsPreviewFocused;

  glm::vec3 GetPositionModifiers();
  glm::vec3 GetScaleModifiers();
  glm::quat GetRotationModifiers();

  bool IsWindowContentFocused();
  void Initialize();
  void DrawGameViewPort();
  void DrawModelSelectWindow();
  void DrawButtonDock();
  std::string GetRelativeSelectedPath();
  void UpdateUI();
  void RenderUI();
};
