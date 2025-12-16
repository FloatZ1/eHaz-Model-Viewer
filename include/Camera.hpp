#ifndef CAMERA_H
#define CAMERA_H

#include <SDL3/SDL_log.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

enum Camera_Movement { MOVING, LOOKING };

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
  // ===== ORBIT CAMERA DATA =====
  glm::vec3 Target = glm::vec3(0.0f); // point we orbit around
  float Distance = 5.0f;              // orbit radius

  bool g_bMoving = false;

  // camera Attributes
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;

  // euler Angles
  float Yaw;
  float Pitch;

  // camera options
  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  // constructors
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
         float pitch = PITCH)
      : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {

    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;

    Distance = glm::length(position - Target); // ORBIT
    updateCameraVectors();
  }

  // view matrix ALWAYS looks at target
  glm::mat4 GetViewMatrix() { return glm::lookAt(Position, Target, Up); }

  // OPTIONAL: disable WASD movement for orbit camera
  void ProcessKeyboard(Camera_Movement Direction, float deltaTime) {

    if (Direction == MOVING) {
      g_bMoving = true;
    }
    if (Direction == LOOKING) {
      g_bMoving = false;
    }
  }

  // ORBIT mouse movement
  void ProcessMouseMovement(float xoffset, float yoffset,
                            GLboolean constrainPitch = true) {
    if (!g_bMoving) {
      xoffset *= MouseSensitivity;
      yoffset *= MouseSensitivity;

      Yaw += xoffset;
      Pitch += yoffset;

      if (constrainPitch) {
        if (Pitch > 89.0f)
          Pitch = 89.0f;
        if (Pitch < -89.0f)
          Pitch = -89.0f;
      }
    } else {
      float panSpeed = 0.002f;

      glm::vec3 pan = (-Right * xoffset + Up * yoffset) * panSpeed;

      Target += pan;
      Position += pan;
    }

    updateCameraVectors();
  }

  // Zoom = change distance
  void ProcessMouseScroll(float yoffset) {
    Distance -= yoffset;
    Distance = glm::clamp(Distance, 1.0f, 100.0f);
    updateCameraVectors();
  }

private:
  void updateCameraVectors() {
    // ORBIT POSITION CALCULATION
    float yawRad = glm::radians(Yaw);
    float pitchRad = glm::radians(Pitch);

    Position.x = Target.x + Distance * cos(pitchRad) * cos(yawRad);
    Position.y = Target.y + Distance * sin(pitchRad);
    Position.z = Target.z + Distance * cos(pitchRad) * sin(yawRad);

    // Direction vectors
    Front = glm::normalize(Target - Position);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
  }
};

#endif
