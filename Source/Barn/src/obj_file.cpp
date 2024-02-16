#include "obj_file.h"

#include <fstream>
#include <sstream>
#include <memory>

std::shared_ptr<Temple::Barn::ObjFile>
Temple::Barn::read_obj_model_file(const char* filename)
{
  std::ifstream file(filename);

  if (!file.is_open())
  {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return nullptr;
  }
  std::shared_ptr<ObjFile> obj_ptr = std::make_shared<ObjFile>();
  std::string              line;
  while (getline(file, line))
  {
    std::istringstream iss(line);
    std::string        prefix;
    iss >> prefix;
    if (prefix == "v")
    {
      // vertex
      Base::vec3_t v;
      iss >> v.x >> v.y >> v.z;
      obj_ptr->coord.push_back(v);
    }
    else if (prefix == "vt")
    {
      // Texture
      Base::vec2_t t;
      iss >> t.u >> t.v;
      obj_ptr->uv.push_back(t);
    }
    else if (prefix == "vn")
    {
      // normal
      Base::vec3_t n;
      iss >> n.x >> n.y >> n.z;
      obj_ptr->normal.push_back(n);
    }
    else if (prefix == "f")
    {
      ObjFace     face;
      std::string vertex;
      int         face_vert_idx = 0;
      while (iss >> vertex)
      {
        std::istringstream vertex_stream(vertex);
        std::string        index;
        int                idx;

        // Vertex index
        getline(vertex_stream, index, '/');
        idx = std::stoi(index);
        face.indices[face_vert_idx].position = idx;

        // Texture index
        if (getline(vertex_stream, index, '/'))
        {
          idx = std::stoi(index);
          face.indices[face_vert_idx].Texture = idx;
        }

        // Normal index
        if (getline(vertex_stream, index))
        {
          idx = std::stoi(index);
          face.indices[face_vert_idx].normal = idx;
        }
        face_vert_idx++;
      }
      obj_ptr->faces.push_back(face);
    }
  }
  return obj_ptr;
}
