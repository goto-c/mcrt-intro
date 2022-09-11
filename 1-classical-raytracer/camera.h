#pragma once
#include <cstdio>

#include "core.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

class Camera
{
 public:
  Camera(const glm::vec3& origin_, const glm::vec3& forward_)
      : m_origin(origin_), m_forward(forward_)

  {
    // TODO: implement this
    // calculate right, up direction from forward direction
    m_right = glm::cross(m_forward, glm::vec3(0, 1, 0));
    m_up = glm::cross(m_right, m_forward);
  }

  // sample ray from pinhole camera
  // ndc(normalized device coordinate): [-1, 1] x [-1, 1]
  Ray sampleRay(const glm::vec2& ndc) const
  {
    // TODO: implement this
    return Ray(m_origin,
               glm::normalize(m_forward + ndc.x * m_right + ndc.y * m_up));
  }

 private:
  glm::vec3 m_origin;   // camera position
  glm::vec3 m_forward;  // camera forward direction
  glm::vec3 m_right;    // camera right direction
  glm::vec3 m_up;       // camera up direction
};
