#pragma once
#include <cmath>

#include "core.h"
#include "glm/detail/qualifier.hpp"
#include "glm/exponential.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"

class Shape
{
 public:
  // find ray intersection
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
};

class Triangle : public Shape
{
 public:
  Triangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2)
      : m_v0(v0), m_v1(v1), m_v2(v2)
  {
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const override
  {
    glm::vec3 e1 = m_v1 - m_v0;
    glm::vec3 e2 = m_v2 - m_v0;
    glm::vec3 alpha = glm::cross(ray.direction, e2);
    float det = glm::dot(e1, alpha);

    float k_eps = 1e-6f;
    if (-k_eps < det && det < k_eps) { return false; }
    float invDet = 1.f / det;
    glm::vec3 r = ray.origin - m_v0;

    float u = glm::dot(alpha, r) * invDet;
    if (u < 0.f || u > 1.f) { return false; }
    glm::vec3 beta = glm::cross(r, e1);

    float v = glm::dot(ray.direction, beta) * invDet;
    if (v < 0.f || u + v > 1.f) { return false; }

    float t = glm::dot(e2, beta) * invDet;
    if (t < 0.f) { return false; }

    info.position = (1 - u - v) * m_v0 + u * m_v1 + v * m_v2;
    info.normal = glm::normalize(glm::cross(e1, e2));
    info.t = t;
    return true;
  }

 private:
  glm::vec3 m_v0;
  glm::vec3 m_v1;
  glm::vec3 m_v2;
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
