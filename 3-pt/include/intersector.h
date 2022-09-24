#pragma once
#include <memory>
#include <vector>

#include "core.h"
#include "primitive.h"

class Intersector
{
 public:
  // find closest ray intersection
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
};

// search all intersectables
// O(N)
class LinearIntersector : public Intersector
{
 public:
  LinearIntersector(Primitive* primitives, int n_primitives)
      : m_primitives(primitives), m_n_primitives(n_primitives)
  {
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const override
  {
    // TODO: implement this
    IntersectInfo tmp;
    float tmax = ray.tmax;
    bool is_hit = false;

    for (int i = 0; i < m_n_primitives; i++) {
      if (m_primitives[i].intersect(ray, tmp) && tmp.t < tmax) {
        info = tmp;
        tmax = tmp.t;
        is_hit = true;
      }
    }
    return is_hit;
  }

 private:
  Primitive* m_primitives;  // array of primitive pointers
  int m_n_primitives;
};
