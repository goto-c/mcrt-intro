#pragma once
#include <cmath>

#include "core.h"
#include "glm/exponential.hpp"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"

class Shape
{
 public:
  // find ray intersection
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
};

class Sphere : public Shape
{
 public:
  Sphere(const glm::vec3& center, float radius)
      : m_center(center), m_radius(radius)
  {
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const override
  {
    // TODO: implement this
    const float b = glm::dot(ray.direction, ray.origin - m_center);
    const float c = glm::dot(ray.origin - m_center, ray.origin - m_center) -
                    m_radius * m_radius;
    const float D = b * b - c;

    if (D < 0) { return false; }

    float t0 = -b - std::sqrt(D);
    float t1 = -b + std::sqrt(D);

    float t = t0;
    if (ray.tmin < t && t < ray.tmax) {
      info.t = t;
      info.position = ray(t);
      info.normal = glm::normalize(info.position - m_center);
      return true;
    }
    t = t1;
    if (ray.tmin < t && t < ray.tmax) {
      info.t = t;
      info.position = ray(t);
      info.normal = glm::normalize(info.position - m_center);
      return true;
    }
    return false;
  }

 private:
  glm::vec3 m_center;  // center of sphere
  float m_radius;      // radius of sphere
};
