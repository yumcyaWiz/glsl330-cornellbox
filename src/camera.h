#ifndef _CAMERA_H
#define _CAMERA_H
#include <cmath>

#include "glm/glm.hpp"

class Camera {
 public:
  glm::vec3 camPos;
  glm::vec3 camForward;
  glm::vec3 camRight;
  glm::vec3 camUp;

 private:
  glm::vec3 lookat;

 public:
  Camera()
      : camPos({278, 273, -900}),
        camForward({0, 0, 1}),
        camRight({1, 0, 0}),
        camUp({0, 1, 0}),
        lookat({0, 0, 0}) {}

  void move(const glm::vec3& v) {
    const float dist = glm::distance(lookat, camPos);
    camPos += v.x * camRight + v.y * camUp + v.z * camForward;
    lookat = camPos + dist * camForward;
  }

  void orbit(float dTheta, float dPhi) {
    // compute current (theta, phi)
    glm::vec3 r = glm::normalize(camPos - lookat);
    float phi = std::atan2(r.z, r.x);
    if (phi < 0) phi += 2 * 3.14;
    float theta = std::acos(r.y);

    // add
    phi += dPhi;
    theta += dTheta;

    // recompute r
    r = glm::vec3(std::cos(phi) * std::sin(theta), std::cos(theta),
                  std::sin(phi) * std::sin(theta));

    const float dist = glm::distance(lookat, camPos);
    camPos = lookat + dist * r;
    camForward = -r;
    camRight = glm::normalize(glm::cross(camForward, glm::vec3(0, 1, 0)));
    camUp = glm::normalize(glm::cross(camRight, camForward));
  }
};

#endif