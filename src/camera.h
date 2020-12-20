#ifndef _CAMERA_H
#define _CAMERA_H
#include <cmath>

#include "glm/glm.hpp"
//
#include "constant.h"

struct alignas(16) CameraBlock {
  alignas(16) glm::vec3 camPos;
  alignas(16) glm::vec3 camForward;
  alignas(16) glm::vec3 camRight;
  alignas(16) glm::vec3 camUp;
  float a;

  CameraBlock(const glm::vec3& camPos, const glm::vec3& camForward,
              const glm::vec3& camRight, const glm::vec3& camUp)
      : camPos(camPos),
        camForward(camForward),
        camRight(camRight),
        camUp(camUp),
        a(1.7071067811865477) {}
};

class Camera {
 public:
  CameraBlock params;

 private:
  glm::vec3 lookat;

 public:
  Camera()
      : params({278, 273, -900}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}),
        lookat({278, 273, 279.6}) {}

  void setFOV(float fov) { params.a = 1.0f / std::tan(0.5f * fov); }

  void move(const glm::vec3& v) {
    // const float dist = glm::distance(lookat, params.camPos);
    params.camPos +=
        v.x * params.camRight + v.y * params.camUp + v.z * params.camForward;
    // lookat = params.camPos + dist * params.camForward;
  }

  void orbit(float dTheta, float dPhi) {
    // compute current (theta, phi)
    glm::vec3 r = glm::normalize(params.camPos - lookat);
    float phi = std::atan2(r.z, r.x);
    if (phi < 0) phi += 2 * PI;
    float theta = std::acos(r.y);

    // add
    phi += dPhi;
    theta += dTheta;

    // recompute r
    r = glm::vec3(std::cos(phi) * std::sin(theta), std::cos(theta),
                  std::sin(phi) * std::sin(theta));

    const float dist = glm::distance(lookat, params.camPos);
    params.camPos = lookat + dist * r;
    params.camForward = -r;
    params.camRight =
        glm::normalize(glm::cross(params.camForward, glm::vec3(0, 1, 0)));
    params.camUp =
        glm::normalize(glm::cross(params.camRight, params.camForward));
  }
};

#endif