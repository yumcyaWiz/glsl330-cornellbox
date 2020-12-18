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

  Primitive createPlane(const glm::vec3& leftCornerPoint,
                        const glm::vec3& right, const glm::vec3& up) {
    Primitive ret;
    ret.type = 1;
    ret.leftCornerPoint = leftCornerPoint;
    ret.right = right;
    ret.up = up;
    return ret;
  }

  std::vector<Primitive> primitives;

  void addPrimitive(const Primitive& primitive) {
    primitives.push_back(primitive);
  }

  void setupCornellBox() {
    Primitive floor =
        createPlane(glm::vec3(0), glm::vec3(0, 0, 559.2), glm::vec3(556, 0, 0));
    addPrimitive(floor);

    Primitive rightWall = createPlane(glm::vec3(0), glm::vec3(0, 548.8, 0),
                                      glm::vec3(0, 0, 559.2));
    addPrimitive(rightWall);

    Primitive leftWall = createPlane(
        glm::vec3(556, 0, 0), glm::vec3(0, 0, 559.2), glm::vec3(0, 548.8, 0));
    addPrimitive(leftWall);

    Primitive ceil = createPlane(glm::vec3(0, 548.8, 0), glm::vec3(556, 0, 0),
                                 glm::vec3(0, 0, 559.2));
    addPrimitive(ceil);

    Primitive backWall = createPlane(
        glm::vec3(0, 0, 559.2), glm::vec3(0, 548.8, 0), glm::vec3(556, 0, 0));
    addPrimitive(backWall);

    Primitive shortBox1 = createPlane(
        glm::vec3(130, 165, 65), glm::vec3(-48, 0, 160), glm::vec3(160, 0, 49));
    addPrimitive(shortBox1);

    Primitive shortBox2 = createPlane(
        glm::vec3(290, 0, 114), glm::vec3(0, 165, 0), glm::vec3(-50, 0, 158));
    addPrimitive(shortBox2);

    Primitive shortBox3 = createPlane(
        glm::vec3(130, 0, 65), glm::vec3(0, 165, 0), glm::vec3(160, 0, 49));
    addPrimitive(shortBox3);

    Primitive shortBox4 = createPlane(
        glm::vec3(82, 0, 225), glm::vec3(0, 165, 0), glm::vec3(48, 0, -160));
    addPrimitive(shortBox4);

    Primitive shortBox5 = createPlane(
        glm::vec3(240, 0, 272), glm::vec3(0, 165, 0), glm::vec3(-158, 0, -47));
    addPrimitive(shortBox5);

    Primitive tallBox1 =
        createPlane(glm::vec3(423, 330, 247), glm::vec3(-158, 0, 49),
                    glm::vec3(49, 0, 159));
    addPrimitive(tallBox1);

    Primitive tallBox2 = createPlane(
        glm::vec3(423, 0, 247), glm::vec3(0, 330, 0), glm::vec3(49, 0, 159));
    addPrimitive(tallBox2);

    Primitive tallBox3 = createPlane(
        glm::vec3(472, 0, 406), glm::vec3(0, 330, 0), glm::vec3(-158, 0, 50));
    addPrimitive(tallBox3);

    Primitive tallBox4 = createPlane(
        glm::vec3(314, 0, 456), glm::vec3(0, 330, 0), glm::vec3(-49, 0, -160));
    addPrimitive(tallBox4);

    Primitive tallBox5 = createPlane(
        glm::vec3(265, 0, 296), glm::vec3(0, 330, 0), glm::vec3(158, 0, -49));
    addPrimitive(tallBox5);

    Primitive light = createPlane(glm::vec3(343, 548.6, 227),
                                  glm::vec3(-130, 0, 0), glm::vec3(0, 0, 105));
    addPrimitive(light);
  }

  void init() {
    for (std::size_t i = 0; i < primitives.size(); ++i) {
      primitives[i].id = i;
    }
  }

 public:
  Scene() {
    setupCornellBox();
    init();
  }
};

#endif