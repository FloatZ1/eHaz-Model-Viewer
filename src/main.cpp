#include <SDL3/SDL_log.h>
#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "DataStructs.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl3.h"
#include "glm/ext/matrix_transform.hpp"
#include <Renderer.hpp>
using namespace boost::interprocess;

constexpr std::string s_ext = ".hzmdl";

constexpr std::string a_ext = ".ahzm";

message_queue mq(open_or_create, "model_select_eHaz", 100, 512);

float deltaTime = 0.0f;

void SetupDockingSpace() {
  // Begin a full-screen window for docking
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                  ImGuiWindowFlags_NoBringToFrontOnFocus |
                  ImGuiWindowFlags_NoNavFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

  ImGui::Begin("DockSpace Window", nullptr, window_flags);

  ImGui::PopStyleVar(2);

  // DockSpace
  ImGuiIO &io = ImGui::GetIO();
  ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
  ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

  ImGui::End();
}

namespace fs = std::filesystem;

using namespace eHazGraphics;

std::string toLower(const std::string &str) {
  std::string out = str;
  std::transform(out.begin(), out.end(), out.begin(), ::tolower);
  return out;
}

void RenderImgui(std::vector<fs::path> filteredFiles, Renderer &renderer) {

  // Start ImGui frame
  ImGui_ImplOpenGL3_NewFrame();

  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // Setup docking space
  SetupDockingSpace();

  // Example: list models in a docked window
  ImGui::Begin("Models");
  for (size_t i = 0; i < filteredFiles.size(); ++i) {
    if (ImGui::Selectable(filteredFiles[i].filename().string().c_str())) {
      // Send selected path via Boost message queue
      mq.send(filteredFiles[i].string().c_str(),
              filteredFiles[i].string().size() + 1, 0);
    }
  }
  ImGui::End();

  // Render
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ModelID loadedModel;
bool isStatic = false;

ShaderComboID g_shIDStaticModel;

ShaderComboID g_shIDAniamtedModel;

void LoadSelectedModel(std::string path, Renderer &renderer) {

  std::string ext = path.substr(path.find_last_of("."));
  renderer.p_bufferManager->ClearBuffer(TypeFlags::BUFFER_ANIMATED_MESH_DATA);

  renderer.p_bufferManager->ClearBuffer(TypeFlags::BUFFER_STATIC_MESH_DATA);

  renderer.p_bufferManager->ClearBuffer(TypeFlags::BUFFER_ANIMATION_DATA);

  renderer.p_bufferManager->ClearBuffer(TypeFlags::BUFFER_STATIC_MATRIX_DATA);

  renderer.p_bufferManager->ClearBuffer(TypeFlags::BUFFER_STATIC_DATA);

  if (ext == a_ext) {
    isStatic = false;
    renderer.p_AnimatedModelManager->ClearSubmittedModelInstances();

    renderer.p_AnimatedModelManager->ClearEverything();
    loadedModel = renderer.p_AnimatedModelManager->LoadAHazModel(path);
    auto model = renderer.p_AnimatedModelManager->GetModel(loadedModel);
    renderer.p_AnimatedModelManager->SetModelShader(model, g_shIDAniamtedModel);

  } else if (ext == s_ext) {
    isStatic = true;
    renderer.p_meshManager->ClearSubmittedModelInstances();

    renderer.p_meshManager->ClearEverything();

    loadedModel = renderer.p_meshManager->LoadHazModel(path);
    auto model = renderer.p_meshManager->GetModel(loadedModel);
    renderer.p_meshManager->SetModelShader(model, g_shIDAniamtedModel);
  }
}
glm::mat4 position(1.0f);

void DrawSelectedModel(Renderer &renderer) {
  position = glm::translate(position, glm::vec3(0.0f, 0.0f, 0.0f));

  if (isStatic) {

    auto model = renderer.p_meshManager->GetModel(loadedModel);

    renderer.SubmitStaticModel(model, position,
                               TypeFlags::BUFFER_STATIC_MESH_DATA);

  } else {

    auto model = renderer.p_AnimatedModelManager->GetModel(loadedModel);

    renderer.SubmitAnimatedModel(model, position);
  }
}

void SetupShaders(Renderer &renderer) {

  g_shIDAniamtedModel = renderer.p_shaderManager->CreateShaderProgramme(
      PROJECT_ROOT_DIR "/assets/animation.vert",
      PROJECT_ROOT_DIR "/assets/shader.frag");
  SDL_Log(PROJECT_ROOT_DIR "/assets/animation.vert");
  g_shIDStaticModel = renderer.p_shaderManager->CreateShaderProgramme(
      PROJECT_ROOT_DIR "/assets/shader.vert",
      PROJECT_ROOT_DIR "/assets/shader.frag");
}

int main(int argc, char *argv[]) {

  Renderer renderer;

  renderer.Initialize(640, 580, "Model Viewer");

  renderer.SetFrameBuffer(renderer.GetMainFBO());
  SetupShaders(renderer);

  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <directory> <ext1> [ext2 ...]\n";
    return 1;
  }

  fs::path directoryPath = argv[1];

  // Store extensions to filter, lowercase for case-insensitive matching
  std::vector<std::string> extensions;
  for (int i = 2; i < argc; ++i) {
    std::string ext = argv[i];
    if (ext[0] != '.')
      ext = "." + ext; // Ensure dot prefix
    extensions.push_back(toLower(ext));
  }

  std::vector<fs::path> filteredFiles;

  // Lambda filter function
  auto matchesExtensions = [&](const fs::path &path) -> bool {
    std::string ext = toLower(path.extension().string());
    return std::find(extensions.begin(), extensions.end(), ext) !=
           extensions.end();
  };

  for (const auto &entry : fs::recursive_directory_iterator(directoryPath)) {
    if (entry.is_regular_file() && matchesExtensions(entry.path())) {
      filteredFiles.push_back(entry.path());
    }
  }

  SetupDockingSpace();
  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;

  while (renderer.shouldQuit) {

    renderer.PollInputEvents();

    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();

    deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

    renderer.UpdateRenderer(deltaTime);
    renderer.SetFrameBuffer(renderer.GetMainFBO());

    // renderer render call
    DrawSelectedModel(renderer);

    renderer.DefaultFrameBuffer();

    RenderImgui(filteredFiles, renderer);

    renderer.SwapBuffers();
  }
}
