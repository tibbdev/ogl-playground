#include "resources.h"

#include <physfs.h>
#include <vector>
#include <string>
#include <iostream>

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
        std::cout << "Failed to mount EXE : " << argv0 << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
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