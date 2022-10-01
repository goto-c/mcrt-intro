#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>

#include "bsdf.h"
#include "bxdf.h"
#include "core.h"
#include "glm/fwd.hpp"
#include "intersector.h"
#include "sampler.h"

#define RAY_EPS 0.01f

class Integrator
{
 public:
  // compute incoming radiance
  virtual glm::vec3 integrate(const Ray& ray, const Intersector* intersector,
                              Sampler& sampler) const = 0;
};

class PathTracing : public Integrator
{
 public:
  PathTracing(uint32_t max_depth) : m_max_depth(max_depth) {}

  glm::vec3 integrate(const Ray& ray_in, const Intersector* intersector,
                      Sampler& sampler) const override
  {
    glm::vec3 radiance = glm::vec3(0.f);
    glm::vec3 throughput = glm::vec3(1.f);
    Ray ray = ray_in;
    // TODO: implement this
    for (int i = 0; i < m_max_depth; i++) {
      IntersectInfo info;

      float p = glm::min(
          glm::max(glm::max(throughput.x, throughput.y), throughput.z), 1.f);

      if (sampler.next_1d() > p) { break; }
      throughput /= p;

      if (intersector->intersect(ray, info)) {
        if (info.primitive->has_emission()) {
          radiance += throughput * info.primitive->material->ke;
          return radiance;
        }

        // compute tangent space basis
        glm::vec3 tangent, bitangent;
        orthonormal_basis(info.normal, tangent, bitangent);

        // setup BSDF
        const std::shared_ptr<BSDF> bsdf = std::make_shared<LambertOnly>(info);

        // sample direction from BSDF
        const glm::vec3 wo =
            world_to_local(-ray.direction, tangent, info.normal, bitangent);
        glm::vec3 f;
        float pdf;
        const glm::vec3 wi =
            bsdf->sampleDirection(sampler.next_2d(), wo, f, pdf);

        // update throughput
        throughput *= f * abs_cos_theta(wi) / pdf;

        ray.direction = local_to_world(wi, tangent, info.normal, bitangent);
        ray.origin = info.position + RAY_EPS * info.normal;
      } else {
        radiance += throughput * glm::vec3(0.f);  // sky color : glm::vec3(1.f)
        return radiance;
      }
    }
    return glm::vec3(0.f);
  }

 private:
  uint32_t m_max_depth;
};
