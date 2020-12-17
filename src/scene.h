#ifndef _SCENE_H
#define _SCENE_H
#include <string>

#include "glm/glm.hpp"

class Scene {
 private:
  struct Primitive {
    int id;                     // 4
    int type;                   // 8
    glm::vec3 center;           // 24
    float radius;               // 28
    glm::vec3 leftCornerPoint;  // 44
    glm::vec3 up;               // 60
    glm::vec3 right;            // 76
    glm::vec3 kd;               // 92
    glm::vec3 le;               // 108
  };

  struct Material {
    glm::vec3 kd;  // 16
    glm::vec3 le;  // 32
  };

  Primitive createSphere(const glm::vec3& center, float radius) {
    Primitive ret;
    ret.type = 0;
    ret.center = center;
    ret.radius = radius;
    return ret;
  }

  Primitive createPlane(const glm::vec3& leftCornerPoint, const glm::vec3& up,
                        const glm::vec3& right) {
    Primitive ret;
    ret.type = 1;
    ret.leftCornerPoint = leftCornerPoint;
    ret.up = up;
    ret.right = right;
    return ret;
  }

  std::vector<Primitive> primitives;

  void setupCornellBox() {}

 public:
  Scene() {}
};

#endif