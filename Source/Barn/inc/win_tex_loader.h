#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include "bonfire_texture_holder.h"

static std::unordered_map<std::string, int> s_texturesLoaded;

uint32_t loadTexture(const char* path);
void saveTexture(const uint8_t* data, int w, int h, int channels);
