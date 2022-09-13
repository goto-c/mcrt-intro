#include <memory>
#include <vector>

#include "shape.h"

std::pair<std::vector<std::shared_ptr<Triangle>>,
          std::vector<std::shared_ptr<Material>>>
load_obj(std::string inputfile);
