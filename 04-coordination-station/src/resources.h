#pragma once

#include <stdint.h>
#include <string>
#include "gltfloader.h"

struct ImageData
{
	uint8_t* pixels = nullptr;
	int		 width = 0;
	int		 height = 0;
	int		 channels = 0;
};

void resources_init(const char* argv0);
std::string resources_load_file_to_string(const std::string& filename);
ImageData resources_load_image(const std::string& filename);
