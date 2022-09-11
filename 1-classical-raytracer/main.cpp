#include "camera.h"
#include "core.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "image.h"
#include "io.h"
#include "shape.h"

int main()
{
  const int WIDTH = 512;
  const int HEIGHT = 512;

  Image image(WIDTH, HEIGHT);
  Camera camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, -1));
  Sphere sphere = Sphere(glm::vec3(0, 0, 0), 1.0f);

  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      glm::vec2 ndc =
          glm::vec2((2.f * i - WIDTH) / WIDTH, (2.f * j - HEIGHT) / HEIGHT);
      ndc.y *= -1.f;
      IntersectInfo info;
      Ray ray = camera.sampleRay(ndc);
      if (sphere.intersect(ray, info)) {
        image.addPixel(i, j, (info.normal + 1.f) / 2.f);
      } else {
        image.addPixel(i, j, glm::vec3(0));
      }
    }
  }
  write_png("output.png", WIDTH, HEIGHT, image.getConstPtr());
  return 0;
}
