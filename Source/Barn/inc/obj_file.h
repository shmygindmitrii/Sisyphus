#pragma once

#include "base_vectors.h"

#include <vector>
#include <memory>

namespace Temple
{
namespace Barn
{
  struct ObjFaceIndex {
    unsigned int position;
    unsigned int texture;
    unsigned int normal;
  };
  struct ObjFace {
#if 0
            unsigned int index_count;
            ObjFaceIndex indices[4];
#else
    ObjFaceIndex indices[3];
#endif
  };

  struct ObjFile {
    std::vector<Base::vec3> coord;
    std::vector<Base::vec2> uv;
    std::vector<Base::vec3> normal;
    std::vector<ObjFace>    faces;
  };
  std::shared_ptr<ObjFile>
  ReadObj(const char* path);
} // namespace Barn
} // namespace Temple
