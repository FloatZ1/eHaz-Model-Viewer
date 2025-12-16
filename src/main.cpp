#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>
#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "Animation/AnimatedModelManager.hpp"
#include "Camera.hpp"

#include "DataStructs.hpp"
#include "FileSystem.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl3.h"
#include "UI.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <Renderer.hpp>

// #define EHAZ_DEBUG

using namespace boost::interprocess;

using namespace eHazGraphics;
constexpr std::string s_ext = ".hzmdl";

constexpr std::string a_ext = ".ahzm";

#define DEAFULT_ASSETS ;

message_queue mq(open_or_create, "model_select_eHaz", 100, 512);

float g_fDeltaTime = 0.0f;

float g_fLastFrame = 0.0f;

ShaderComboID g_siShader;

static bool g_bIsFocused = false;

static bool g_bIsMoving = false;

Camera g_camera;

void processInput(Window *c_window, bool &quit, Camera &camera) {

  SDL_Window *window = c_window->GetWindowPtr();
  // Delta time calculation using performance counters

  // Static mouse state
  static bool firstMouse = true;
  static float lastX = 0.0f;
  static float lastY = 0.0f;

  SDL_Event event;
  const bool *key_states = SDL_GetKeyboardState(nullptr);

  if (key_states[SDL_SCANCODE_LSHIFT]) {
    g_bIsMoving = true;

  } else {

    g_bIsMoving = false;
  }

  while (SDL_PollEvent(&event)) {

    ImGui_ImplSDL3_ProcessEvent(&event);
    static short l_sClickCount = 0;
    if (g_bIsMoving && g_bIsFocused)
      camera.ProcessKeyboard(MOVING, static_cast<float>(g_fDeltaTime));
    else if (!g_bIsMoving && g_bIsFocused) {
      camera.ProcessKeyboard(LOOKING, static_cast<float>(g_fDeltaTime));
    }

    if (g_bIsFocused) {

      if (!c_window->isCursorLocked())
        c_window->ToggleMouseCursor();
    } else {
      if (c_window->isCursorLocked()) {
        c_window->ToggleMouseCursor();
      }
    }

    switch (event.type) {
    case SDL_EVENT_QUIT:
      quit = true;
      break;

    case SDL_EVENT_MOUSE_MOTION: {
      if (c_window->isCursorLocked()) {
        float xoffset = static_cast<float>(event.motion.xrel);
        float yoffset = static_cast<float>(event.motion.yrel);

        yoffset = yoffset;

        xoffset *= 10;

        yoffset *= 10;

        if (xoffset != lastX && yoffset != lastY)
          camera.ProcessMouseMovement(xoffset, yoffset, false);
        lastX = xoffset;
        lastY = yoffset;
      }
      break;
    }

    // --- Added mouse scroll support for zoom ---
    case SDL_EVENT_MOUSE_WHEEL: {
      float yoffset = static_cast<float>(event.wheel.y);
      camera.ProcessMouseScroll(yoffset);
      camera.Zoom += yoffset;
      break;
    }

    case SDL_EVENT_WINDOW_RESIZED: {
      int width = event.window.data1;
      int height = event.window.data2;
      c_window->SetDimensions(width, height);
      glViewport(0, 0, width, height);
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
      if (event.button.button == SDL_BUTTON_LEFT &&
          CSelectUI::s_bIsPreviewFocused) {

        g_bIsFocused = true;
      }
    } break;

    case SDL_EVENT_MOUSE_BUTTON_UP: {

      if (event.button.button == SDL_BUTTON_LEFT &&
          CSelectUI::s_bIsPreviewFocused)

        g_bIsFocused = false;
    }
    default:
      break;
    }
  }
}
struct camData {
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);
};

std::shared_ptr<Model> g_sptrModel;

void LoadSelectedModel(std::string path);

#define DEBUGGING_ARGS
int main(int argc, char *argv[]) {

#ifdef DEBUGGING_ARGS

  static const char *fake_argv[] = {"./eHazEngine",
                                    "--root",
                                    "/home/floatz/Projects/personal/c++/ENGINE/"
                                    "eHaz Model Viewer/eHaz-Model-Viewer/",
                                    "--ext",
                                    ".hzmdl",
                                    ".ahzm",
                                    nullptr};

  argc = 6;
  argv = const_cast<char **>(fake_argv);

#endif

  // make it set the root from the arguments

  CFileSystem l_FileSystem;

  l_FileSystem.SetFromCommandLine(argc, argv);

  auto l_vstrFiles = l_FileSystem.GetFilesFromRoot();

  eHazGraphics::Renderer l_renderer;
  l_renderer.Initialize(720, 860, "Model viewer");

  l_renderer.p_bufferManager->BeginWritting();

  CSelectUI l_SelectUI;
  l_SelectUI.Initialize();

  l_SelectUI.m_vsFiles = l_vstrFiles;

  uint AlbedoTexture = l_renderer.p_materialManager->LoadTexture(
      PROJECT_ROOT_DIR "/assets/missing.png");

  uint materialID = l_renderer.p_materialManager->CreatePBRMaterial(
      AlbedoTexture, AlbedoTexture, AlbedoTexture, AlbedoTexture, "default_m");

  auto mat = l_renderer.p_materialManager->SubmitMaterials();

  SBufferRange l_brMaterials = l_renderer.p_bufferManager->InsertNewDynamicData(
      mat.first.data(), mat.first.size() * sizeof(PBRMaterial),
      TypeFlags::BUFFER_TEXTURE_DATA);

  g_siShader = l_renderer.p_shaderManager->CreateShaderProgramme(
      PROJECT_ROOT_DIR "/assets/shader.vert",
      PROJECT_ROOT_DIR "/assets/shader.frag");

  glm::mat4 projection =
      glm::perspective(glm::radians(g_camera.Zoom),
                       (float)l_renderer.p_window->GetWidth() /
                           (float)l_renderer.p_window->GetHeight(),
                       0.1f, 100.0f);

  camData l_cdFinalData{g_camera.GetViewMatrix(), projection};

  SBufferRange l_brCameraDataLocation = l_renderer.SubmitDynamicData(
      &l_cdFinalData, sizeof(l_cdFinalData), TypeFlags::BUFFER_CAMERA_DATA);

  std::string testPath = PROJECT_ROOT_DIR "/assets/test.hzmdl";
  // SDL_Log(testPath.c_str());
  ModelID l_midTestModel =
      eHazGraphics::Renderer::p_meshManager->LoadHazModel(testPath);

  g_sptrModel = l_renderer.p_meshManager->GetModel(l_midTestModel);

  glm::mat4 pos = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

  Renderer::p_meshManager->SetModelShader(g_sptrModel, g_siShader);

  // Renderer::r_instance->SubmitStaticModel(g_sptrModel, pos,
  //                                        TypeFlags::BUFFER_STATIC_MESH_DATA);

  pos = glm::translate(pos, glm::vec3(0.0f, 0.0f, 0.0f));

  Renderer::r_instance->SubmitStaticModel(g_sptrModel, pos,
                                          TypeFlags::BUFFER_STATIC_MESH_DATA);

  auto l_vdrRanges = l_renderer.p_renderQueue->SubmitRenderCommands();

  std::string l_strLastPath = l_vstrFiles[0];

  while (l_renderer.shouldQuit == false) {
    static uint64_t lastCounter = SDL_GetPerformanceCounter();
    uint64_t currentCounter = SDL_GetPerformanceCounter();

    g_fDeltaTime =
        double(currentCounter - lastCounter) / SDL_GetPerformanceFrequency();
    lastCounter = currentCounter;

    processInput(l_renderer.p_window.get(), l_renderer.shouldQuit, g_camera);

    projection = glm::perspective(glm::radians(g_camera.Zoom),
                                  (float)l_renderer.p_window->GetWidth() /
                                      (float)l_renderer.p_window->GetHeight(),
                                  0.1f, 100.0f);

    l_cdFinalData = {g_camera.GetViewMatrix(), projection};

    l_renderer.UpdateDynamicData(l_brCameraDataLocation, &l_cdFinalData,
                                 sizeof(l_cdFinalData));

    l_renderer.UpdateRenderer(g_fDeltaTime);

    Renderer::r_instance->SubmitStaticModel(g_sptrModel, pos,
                                            TypeFlags::BUFFER_STATIC_MESH_DATA);

    l_vdrRanges = Renderer::p_renderQueue->SubmitRenderCommands();

    l_renderer.SetFrameBuffer(l_renderer.GetMainFBO());

    l_renderer.RenderFrame(l_vdrRanges);

    l_renderer.DefaultFrameBuffer();

    l_SelectUI.RenderUI();

    l_renderer.SwapBuffers();

    l_renderer.UpdateDynamicData(l_brMaterials, mat.first.data(),
                                 mat.first.size() * sizeof(PBRMaterial));

    if (l_strLastPath != l_SelectUI.m_sSelectedFile &&
        l_SelectUI.m_sSelectedFile != "") {

      LoadSelectedModel(l_FileSystem.root.string() +
                        l_SelectUI.m_sSelectedFile);

      SDL_Log(("last path: " + l_strLastPath).c_str());
      SDL_Log(("selected path: " + l_SelectUI.m_sSelectedFile).c_str());
      l_strLastPath = l_SelectUI.m_sSelectedFile;
    }

    if (l_SelectUI.m_bFinished || l_SelectUI.m_bCanceled) {

      l_renderer.shouldQuit = true;
    }
  }

  if (l_SelectUI.m_bFinished) {

    // send the final path
  }
}

void LoadSelectedModel(std::string path) {

  auto &renderer = Renderer::r_instance;

  renderer->p_bufferManager->ClearBuffer(TypeFlags::BUFFER_STATIC_MESH_DATA);

  renderer->p_bufferManager->ClearBuffer(TypeFlags::BUFFER_STATIC_MATRIX_DATA);

  renderer->p_bufferManager->ClearBuffer(TypeFlags::BUFFER_ANIMATED_MESH_DATA);
  renderer->p_bufferManager->ClearBuffer(TypeFlags::BUFFER_ANIMATION_DATA);

  renderer->p_meshManager->ClearEverything();
  renderer->p_AnimatedModelManager->ClearEverything();
  renderer->p_bufferManager->ClearBuffer(TypeFlags::BUFFER_STATIC_DATA);

  renderer->p_renderQueue->ClearDynamicCommands();
  renderer->p_renderQueue->ClearStaticCommnads();

  ModelID l_mID = renderer->p_meshManager->LoadHazModel(path);
  g_sptrModel = renderer->p_meshManager->GetModel(l_mID);
  Renderer::p_meshManager->SetModelShader(g_sptrModel, g_siShader);
}
