#pragma once

#include <stdint.h>
#include <string>
#include "gltfloader.h"

#include "tiny_gltf.h"

struct ImageData
{
	uint8_t* pixels = nullptr;
	int		 width = 0;
	int		 height = 0;
	int		 channels = 0;
};

struct Material
{
	glm::vec4 baseColorFactor = glm::vec4(1.0f);
	glm::vec3 emissiveFactor = glm::vec3(0.0f);
	float emissiveStrength = 1.0f;
	uint32_t baseColorTexture = 0; // OpenGL ID
};

struct Primitive
{
	uint32_t vao;
	uint32_t indx_cnt;
	uint32_t type;
	Material material;
};

struct ModelData
{
	tinygltf::Model			model; 
	std::vector<Primitive>	primitives;
	std::vector<uint32_t>	textures;
	uint32_t				vao;
	uint32_t				vbo;
	uint32_t				ebo;
	uint32_t				indx_cnt = 0;
	uint32_t				type = 0;
};

void resources_init(const char* argv0);
std::string resources_load_file_to_string(const std::string& filename);
ImageData resources_load_image(const std::string& filename);
ModelData resources_load_model_glb(const std::string& filename);
