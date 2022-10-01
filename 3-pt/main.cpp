#include <bits/types/time_t.h>
#include <time.h>

#include <memory>

#include "camera.h"
#include "core.h"
#include "glm/fwd.hpp"
#include "image.h"
#include "integrator.h"
#include "intersector.h"
#include "io.h"
#include "primitive.h"
#include "sampler.h"
#include "scene.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "tiny_obj_loader.h"

#define RAY_EPS 0.01f

int main()
{
  time_t t_begin;
  time(&t_begin);

  Scene scene;
  scene.loadObj("./data/head_with_light/head_with_light.obj");

  const int width = 512;
  const int height = 512;
  const int n_samples = 1000;
  spdlog::info("[Main] Sample: {}", n_samples);

  Image image(width, height);
  Camera camera(glm::vec3(0, 0, 0.5), glm::vec3(0, 0, -1));

  LinearIntersector intersector(scene.m_primitives.data(),
                                scene.m_primitives.size());

  Sampler sampler(12);
  PathTracing integrator(10);

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

        glm::vec3 color = integrator.integrate(ray, &intersector, sampler);
        image.addPixel(i, j, color);
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
