#include "obj_file.h"

#include <fstream>
#include <sstream>
#include <memory>

std::shared_ptr<Temple::Barn::ObjFile>
Temple::Barn::ReadObj(const char* filename)
{
  std::ifstream file(filename);

  if (!file.is_open())
  {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return nullptr;
  }
  std::shared_ptr<ObjFile> objPtr = std::make_shared<ObjFile>();
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
      objPtr->coord.push_back(v);
    }
    else if (prefix == "vt")
    {
      // texture
      Base::vec2_t t;
      iss >> t.u >> t.v;
      objPtr->uv.push_back(t);
    }
    else if (prefix == "vn")
    {
      // normal
      Base::vec3_t n;
      iss >> n.x >> n.y >> n.z;
      objPtr->normal.push_back(n);
    }
    else if (prefix == "f")
    {
      ObjFace     face;
      std::string vertex;
      int         faceVertIdx = 0;
      while (iss >> vertex)
      {
        std::istringstream vertexStream(vertex);
        std::string        index;
        int                idx;

        // Vertex index
        getline(vertexStream, index, '/');
        idx = std::stoi(index);
        face.indices[faceVertIdx].position = idx;

        // Texture index
        if (getline(vertexStream, index, '/'))
        {
          idx = std::stoi(index);
          face.indices[faceVertIdx].texture = idx;
        }

        // Normal index
        if (getline(vertexStream, index))
        {
          idx = std::stoi(index);
          face.indices[faceVertIdx].normal = idx;
        }
        faceVertIdx++;
      }
      objPtr->faces.push_back(face);
    }
  }
  return objPtr;
}
