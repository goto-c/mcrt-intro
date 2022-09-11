#include "image.h"
#include "io.h"

int main()
{
  const int WIDTH = 512;
  const int HEIGHT = 512;
  Image image(WIDTH, HEIGHT);
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      image.addPixel(i, j, glm::vec3(1.0, 1.0, 1.0));
    }
  }
  write_png("output.png", WIDTH, HEIGHT, image.getConstPtr());
  return 0;
}
