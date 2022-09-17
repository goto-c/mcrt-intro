#include "obj.h"

#include <iostream>
#include <memory>
#include <vector>

#include "glm/detail/qualifier.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "shape.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "tiny_obj_loader.h"

std::pair<std::vector<std::shared_ptr<Triangle>>,
          std::vector<std::shared_ptr<Material>>>
load_obj(std::string inputfile)
{
  std::pair<std::vector<std::shared_ptr<Triangle>>,
            std::vector<std::shared_ptr<Material>>>
      ret;

  tinyobj::ObjReaderConfig reader_config;
  reader_config.mtl_search_path = "./input/head/";

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(inputfile, reader_config)) {
    if (!reader.Error().empty()) {
      std::cerr << "TinyObjReader: " << reader.Error();
    }
    exit(1);
  }

  if (!reader.Warning().empty()) {
    std::cout << "TinyObjReader: " << reader.Warning();
  }

  auto& attrib = reader.GetAttrib();
  auto& shapes = reader.GetShapes();
  auto& materials = reader.GetMaterials();

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

      std::vector<glm::vec3> vertices(3);
      std::vector<glm::vec3> normals(3);   // per vertices
      std::vector<glm::vec2> texcoord(3);  // per vertices

      // Loop over vertices in the face.
      bool has_normal = false;
      bool has_tex = false;
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
        tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
        tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

        vertices[v] = glm::vec3(vx, vy, vz);
        // Check if `normal_index` is zero or positive. negative = no normal
        // data
        if (idx.normal_index >= 0) {
          has_normal = true;
          tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
          tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
          tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

          normals[v] = glm::vec3(nx, ny, nz);
        }

        // Check if `texcoord_index` is zero or positive. negative = no texcoord
        // data
        if (idx.texcoord_index >= 0) {
          has_tex = true;
          tinyobj::real_t tx =
              attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
          tinyobj::real_t ty =
              attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

          texcoord[v] = glm::vec2(tx, ty);
        }

        // Optional: vertex colors
        // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
        // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
        // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
      }
      index_offset += fv;

      if (!has_normal) {
        glm::vec3 e1 = glm::normalize(vertices[1] - vertices[0]);
        glm::vec3 e2 = glm::normalize(vertices[2] - vertices[0]);
        normals[0] = glm::normalize(glm::cross(e1, e2));
        normals[1] = normals[0];
        normals[2] = normals[0];
      }
      ret.first.push_back(std::make_shared<Triangle>(vertices[0], vertices[1],
                                                     vertices[2], normals[0],
                                                     normals[1], normals[2]));

      int material_id = shapes[s].mesh.material_ids[f];
      const tinyobj::material_t mat = materials[material_id];
      const glm::vec3 kd =
          glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
      const glm::vec3 ks =
          glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
      ret.second.push_back(std::make_shared<Material>(kd, ks, 0.01f));
      // per-face material
      // shapes[s].mesh.material_ids[f];
    }
  }
  spdlog::info("number of triangles: {}", ret.first.size());

  return ret;
}
