#include <memory>

#include "camera.h"
#include "core.h"
#include "glm/exponential.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "image.h"
#include "intersector.h"
#include "io.h"
#include "primitive.h"
#include "sampler.h"
#include "shape.h"

#define RAY_EPS (0.001f)

int main()
{
  // image size
  const int WIDTH = 512;
  const int HEIGHT = 512;

  // settings
  Image image(WIDTH, HEIGHT);
  Camera camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, -1));
  glm::vec3 sun_direction = glm::normalize(glm::vec3(1.f, 1.f, 1.f));
  Sampler sampler(0);

  // Shape
  const auto sphere0 = std::make_shared<Sphere>(glm::vec3(-1.f, 0.f, 0.f), 1.f);
  const auto sphere1 = std::make_shared<Sphere>(glm::vec3(1.f, 0.f, 0.f), 1.f);
  const auto sphere2 =
      std::make_shared<Sphere>(glm::vec3(0.f, 0.f, -glm::sqrt(3)), 1.f);
  const auto floor = std::make_shared<Sphere>(glm::vec3(0, -10001, 0), 10000);
  const auto tri =
      std::make_shared<Triangle>(glm::vec3(-1, -1, 0), glm::vec3(1, -1, 0),
                                 glm::vec3(0, glm::sqrt(3) - 1, 0));

  // Material
  const auto blue = std::make_shared<Material>(glm::vec3(0.2f, 0.2f, 0.5f),
                                               glm::vec3(0.5f), 0.01f);
  const auto red = std::make_shared<Material>(glm::vec3(0.5f, 0.2f, 0.2f),
                                              glm::vec3(1.f), 0.01f);
  const auto yellow = std::make_shared<Material>(glm::vec3(0.5f, 0.5f, 0.2f),
                                                 glm::vec3(1.f), 0.01f);
  const auto white = std::make_shared<Material>(glm::vec3(0.8f, 0.8f, 0.8f),
                                                glm::vec3(0), 0.01f);

  // Primitive
  const auto sphere0_primitive = std::make_shared<Primitive>(sphere0, blue);
  const auto sphere1_primitive = std::make_shared<Primitive>(sphere1, red);
  const auto sphere2_primitive = std::make_shared<Primitive>(sphere2, yellow);
  const auto tri_primitive = std::make_shared<Primitive>(tri, yellow);
  const auto floor_primitive = std::make_shared<Primitive>(floor, white);

  std::vector<std::shared_ptr<Primitive>> primitives;
  // primitives.push_back(sphere0_primitive);
  // primitives.push_back(sphere1_primitive);
  // primitives.push_back(sphere2_primitive);
  primitives.push_back(tri_primitive);
  primitives.push_back(floor_primitive);

  LinearIntersector intersector(primitives);

// calculate pixel color for all pixels
#pragma omp parallel for collapse(2)
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      for (int k = 0; k < 16; k++) {
        glm::vec2 ndc =
            glm::vec2((2.f * (i + sampler.next_1d()) - WIDTH) / WIDTH,
                      (2.f * (j + sampler.next_1d()) - HEIGHT) / HEIGHT);
        ndc.y *= -1.f;

        Ray ray = camera.sampleRay(ndc);
        IntersectInfo info;

        if (intersector.intersect(ray, info)) {
          // check if that surface is shadow
          float shadow = 1.f;
          Ray shadow_ray =
              Ray(info.position + info.normal * RAY_EPS, sun_direction);
          IntersectInfo shadow_info;
          if (intersector.intersect(shadow_ray, shadow_info)) { shadow = 0.2f; }

          glm::vec3 wh = glm::normalize(sun_direction - ray.direction);

          glm::vec3 lambert_color =
              info.primitive->material->kd *
              glm::max(glm::dot(sun_direction, info.normal), 0.f);
          glm::vec3 phong_color =
              info.primitive->material->ks *
              glm::pow(glm::max(glm::dot(info.normal, wh), 0.f),
                       1.f / info.primitive->material->roughness);

          glm::vec3 color = lambert_color + phong_color;

          image.addPixel(i, j, color * shadow);
        } else {
          image.addPixel(i, j, glm::vec3(0));
        }
      }
    }
  }

  image.divide(16.f);

  write_png("output.png", WIDTH, HEIGHT, image.getConstPtr());
  return 0;
}
