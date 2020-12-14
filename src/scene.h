#ifndef _SCENE_H
#define _SCENE_H
#include <string>

#include "glm/glm.hpp"

struct Primitive {
  int id;
  int type;
  glm::vec3 center;
  float radius;
  glm::vec3 leftCornerPoint;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 kd;
  glm::vec3 le;
};

class Scene {
 public:
  Scene() {}
};

#endif