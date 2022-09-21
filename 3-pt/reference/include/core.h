#pragma once

#include "glm/glm.hpp"
#include "texture.h"

struct Ray {
  glm::vec3 origin;     // ray origin
  glm::vec3 direction;  // ray direction
  float tmin = 0.0f;    // minimum hit distance
  mutable float tmax = 1e9f;
  ;  // maximum hit distance

  Ray() {}
  Ray(const glm::vec3& origin_, const glm::vec3& direction_)
      : origin(origin_), direction(direction_)
  {
  }

  // return position on the ray with distance t
  glm::vec3 operator()(float t) const { return origin + t * direction; }
};

// forward declaration
struct Primitive;

struct IntersectInfo {
  float t;                     // hit distance
  glm::vec3 position;          // hit position
  glm::vec3 normal;            // hit normal
  glm::vec2 texcoord;          // hit texcoord
  const Primitive* primitive;  // hit primitive pointer
};

struct Material {
  glm::vec3 kd = glm::vec3(0.0f);   // diffuse color
  const Texture* kd_tex = nullptr;  // diffuse texture

  glm::vec3 ks = glm::vec3(0.0f);   // specular color
  const Texture* ks_tex = nullptr;  // specular texture

  glm::vec3 ke = glm::vec3(0.0f);   // emission color
  const Texture* ke_tex = nullptr;  // emission texture

  float roughness = 1.0f;  // specular roughness

  Material() {}
};

// convert spherical coordinate to cartesian coordinate
inline glm::vec3 spherical_to_cartesian(float phi, float theta)
{
  const float sin_theta = glm::sin(theta);
  return glm::vec3(glm::cos(phi) * sin_theta, glm::cos(theta),
                   glm::sin(phi) * sin_theta);
}

// Duff, T., Burgess, J., Christensen, P., Hery, C., Kensler, A., Liani, M., &
// Villemin, R. (2017). Building an orthonormal basis, revisited. JCGT, 6(1).
inline void orthonormal_basis(const glm::vec3& normal, glm::vec3& tangent,
                              glm::vec3& bitangent)
{
  float sign = std::copysignf(1.0f, normal.z);
  const float a = -1.0f / (sign + normal.z);
  const float b = normal.x * normal.y * a;
  tangent = glm::vec3(1.0f + sign * normal.x * normal.x * a, sign * b,
                      -sign * normal.x);
  bitangent = glm::vec3(b, sign + normal.y * normal.y * a, -normal.y);
}

inline glm::vec3 world_to_local(const glm::vec3& v, const glm::vec3& t,
                                const glm::vec3& n, const glm::vec3& b)
{
  return glm::vec3(glm::dot(v, t), glm::dot(v, n), glm::dot(v, b));
}

inline glm::vec3 local_to_world(const glm::vec3& v, const glm::vec3& t,
                                const glm::vec3& n, const glm::vec3& b)
{
  return glm::vec3(v.x * t.x + v.y * n.x + v.z * b.x,
                   v.x * t.y + v.y * n.y + v.z * b.y,
                   v.x * t.z + v.y * n.z + v.z * b.z);
}