#pragma once

#include "tiny_gltf.h"

#include <string>
#include <stdbool.h>

bool loadModel(tinygltf::Model &model, const std::string& filename);
