#ifndef _CAMERA_H
#define _CAMERA_H

#include "glm/glm.hpp"

class Camera {
 public:
  glm::vec3 camPos;
  glm::vec3 camForward;
  glm::vec3 camRight;
  glm::vec3 camUp;

 private:
  glm::vec3 lookat;
  float theta;
  float phi;

 public:
  Camera()
      : camPos({278, 273, -900}),
        camForward({0, 0, 1}),
        camRight({1, 0, 0}),
        camUp({0, 1, 0}) {}

  void move(const glm::vec3& v) {
    camPos += v.x * camRight + v.y * camUp + v.z * camForward;
  }
};

#endif