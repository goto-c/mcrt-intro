#include <bits/types/time_t.h>
#include <time.h>

#include <memory>

#include "camera.h"
#include "core.h"
#include "glm/fwd.hpp"
#include "image.h"
#include "intersector.h"
#include "io.h"
#include "obj.h"
#include "primitive.h"
#include "sampler.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "tiny_obj_loader.h"

#define RAY_EPS 0.01f

int main()
{
  time_t t_begin;
  time(&t_begin);

  std::string input_obj_file_path = "./data/CornellBox/CornellBox-Original.obj";
  // std::string input_obj_file_path = "./data/head/head.OBJ";
  auto obj_tri_mtl = load_obj(input_obj_file_path);

  const int width = 512;
  const int height = 512;
  const int n_samples = 100;
  spdlog::info("[Main] Sample: {}", n_samples);

  Image image(width, height);
  Camera camera(glm::vec3(0, 1, 3), glm::vec3(0, 0, -1));

  // const auto sphere = std::make_shared<Sphere>(glm::vec3(0), 1.0f);
  // const auto floor =
  //     std::make_shared<Sphere>(glm::vec3(0, -10001, 0), 10000.0f);
  // const auto white = std::make_shared<Material>(
  //     glm::vec3(0.8f), glm::vec3(0.0f), glm::vec3(0.f), 1.0f);
  // const auto green = std::make_shared<Material>(
  //     glm::vec3(0.2f, 0.8f, 0.2f), glm::vec3(0.8f), glm::vec3(0.f), 0.01f);

  // std::vector<std::shared_ptr<Primitive>> primitives;
  // primitives.push_back(std::make_shared<Primitive>(sphere, green));
  // primitives.push_back(std::make_shared<Primitive>(floor, white));

  std::vector<std::shared_ptr<Primitive>> obj_primitives;
  for (int i = 0; i < obj_tri_mtl.first.size(); i++) {
    obj_primitives.push_back(std::make_shared<Primitive>(
        obj_tri_mtl.first[i], obj_tri_mtl.second[i]));
  }

  LinearIntersector intersector(obj_primitives);

  Sampler sampler(12);

  glm::vec3 sun_direction = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));

#pragma omp parallel for collapse(2)
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      for (int k = 0; k < n_samples; ++k) {
        glm::vec2 ndc =
            glm::vec2((2.0f * (i + sampler.next_1d()) - width) / height,
                      (2.0f * (j + sampler.next_1d()) - height) / height);
        ndc.y *= -1.0f;

        // sample ray from camera
        const Ray ray = camera.sampleRay(ndc);

        IntersectInfo info;
        if (intersector.intersect(ray, info)) {
          // const glm::vec3 wi = sun_direction;
          // const glm::vec3 wo = -ray.direction;
          // const glm::vec3 wh = glm::normalize(wo + wi);

          glm::vec3 wi_local = sample_hemisphere(sampler.next_2d());

          glm::vec3 tangent, bitangent;
          orthonormal_basis(info.normal, tangent, bitangent);
          glm::vec3 wi_global =
              local_to_world(wi_local, tangent, info.normal, bitangent);
          Ray shadow_ray(info.position + info.normal * RAY_EPS, wi_global);
          IntersectInfo shadow_info;
          float le = 0.f;
          if (!intersector.intersect(shadow_ray, shadow_info)) { le = 1.f; }
          // glm::vec3 f = glm::vec3(1.f / M_PI);
          glm::vec3 f = info.primitive->material->kd / M_PIf;
          float cos = glm::dot(wi_global, info.normal);
          float pdf = 1.f / (2 * M_PI);

          glm::vec3 color = f * le * cos / pdf;
          image.addPixel(i, j, color);
        } else {
          // ray doesn't hit anything
          image.addPixel(i, j, glm::vec3(1.0f));
        }
      }
    }
  }
  image.divide(n_samples);

  image.post_process();
  write_png("output.png", width, height, image.getConstPtr());

  time_t t_end;
  time(&t_end);

  spdlog::info("[Main] Time: {} [s]", t_end - t_begin);

  return 0;
}
