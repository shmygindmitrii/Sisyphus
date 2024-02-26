#pragma once

#include "base_vectors.h"

#include <vector>
#include <memory>

namespace Sisyphus
{
namespace Util
{
    struct ObjFaceIndex {
        unsigned int position;
        unsigned int texture;
        unsigned int normal;
    };
    struct ObjFace {
        ObjFaceIndex indices[3];
    };

    struct ObjFile {
        std::vector<Base::vec3_t> coord;
        std::vector<Base::vec2_t> uv;
        std::vector<Base::vec3_t> normal;
        std::vector<ObjFace>      faces;
    };
    std::shared_ptr<ObjFile>
    read_obj_model_file(const char* path);
} // namespace Util
} // namespace Sisyphus
