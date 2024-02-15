#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include "bonfire_texture_holder.h"

static std::unordered_map<std::string, int> s_textures_loaded;

uint32_t
load_texture_win(const char* path);
