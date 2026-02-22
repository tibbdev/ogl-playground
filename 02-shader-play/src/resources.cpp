#include "resources.h"

#include <physfs.h>
#include <vector>
#include <string>

void resources_init(const char* argv0) {
    PHYSFS_init(argv0);

    // 1. Mount the executable itself (it will find the appended ZIP)
    PHYSFS_mount(PHYSFS_getBaseDir(), NULL, 1);

    // 2. Also mount the source folders (so changes show up without rebuilding)
    // Only do this during development!
    PHYSFS_mount("../shaders", "/shaders", 0);
    PHYSFS_mount("../assets", "/assets", 0);
}

std::string resources_load_file_to_string(const std::string& filename) {
    if (!PHYSFS_exists(filename.c_str())) return "";

    PHYSFS_File* file = PHYSFS_openRead(filename.c_str());
    PHYSFS_sint64 size = PHYSFS_fileLength(file);
    
    std::string content(size, '\0');
    PHYSFS_readBytes(file, &content[0], size);
    PHYSFS_close(file);

    return content;
}