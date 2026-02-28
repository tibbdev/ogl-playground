#include "resources.h"

#include <physfs.h>
#include <vector>
#include <string>
#include <iostream>

#include "stb_image.h"

void resources_init(const char* argv0) 
{
    std::cout << "Initialising Resources..." << std::endl;
    if (!PHYSFS_init(argv0)) 
    {
        std::cout << "PhysFS Error: " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) << std::endl;
    }

    std::cout << "argv[0] := " << argv0 << std::endl;


    // PhysFS directories
    if (!PHYSFS_mount(argv0, "/", 1))
    {
        std::cout << "Failed to mount EXE : " << argv0 << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) << std::endl;
    }

    // 2. Also mount the source folders (so changes show up without rebuilding)
    // Only do this during development!
    PHYSFS_mount("./shaders", "shaders", 1);
    PHYSFS_mount("./assets", "assets", 1);

    char** rc = PHYSFS_enumerateFiles("/");
    for (char** filename = rc; *filename != NULL; filename++) {
        std::cout << "PhysFS sees: [" << *filename << "]" << std::endl;
    }
    PHYSFS_freeList(rc);

    std::cout << "...done." << std::endl;
}

std::string resources_load_file_to_string(const std::string& filename) {
    if (!PHYSFS_exists(filename.c_str())) 
    {
        std::cout << "PHYSFS File Not Found! [" << filename << "]" << std::endl;
        return "";
    }

    PHYSFS_File* file = PHYSFS_openRead(filename.c_str());
    PHYSFS_sint64 size = PHYSFS_fileLength(file);
    
    std::string content(size, '\0');
    PHYSFS_readBytes(file, &content[0], size);
    PHYSFS_close(file);

    return content;
}

ImageData resources_load_image(const std::string& filename)
{
    ImageData result;

    PHYSFS_File *file = PHYSFS_openRead(filename.c_str());

    if (!file)
    {
        std::cout << "Failed to Open File : " << filename << std::endl;
    }
    else
    {
        // Read the damned file
        PHYSFS_sint64 size = PHYSFS_fileLength(file);
        std::vector<uint8_t> buffer(size);

        PHYSFS_readBytes(file, buffer.data(), size);
        PHYSFS_close(file);

        // Load that image baby!
        stbi_set_flip_vertically_on_load(false);
        result.pixels = stbi_load_from_memory(
            buffer.data(), 
            static_cast<int>(size),
            &result.width, &result.height, &result.channels, 4
        );

        if (!result.pixels)
        {
            std::cout << "STB_ERROR :: " << stbi_failure_reason() << " for " << filename << std::endl;
        }
    }

    return result;
}

ModelData resources_load_model_glb(const std::string& filename)
{
    ModelData result;

    tinygltf::TinyGLTF loader;

    std::string file_content = resources_load_file_to_string(filename);
    std::string err;
    std::string warn;

    if (!file_content.empty())
    {
        loader.LoadBinaryFromMemory(&result.model, &err, &warn, (const unsigned char*)file_content.c_str(), file_content.length());
    }

    if (!warn.empty())
    {
        std::cout << "RESOURCES :: Load .glb : WARNING : " << warn << std::endl;
    }
    if (!err.empty())
    {
        std::cout << "RESOURCES :: Load .glb : ERROR: " << warn << std::endl;
    }

    return result;
}
