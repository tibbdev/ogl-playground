cmake_minimum_required(VERSION 3.14)
project(shader-play-ogl LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- 1. SDL2 Setup ---
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.0
)

# --- 2. GLEW Setup ---
FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.3.1
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)

FetchContent_Declare(
    tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG v2.9.3
)

# --- 3. Make Available (This handles the download) ---
FetchContent_MakeAvailable(SDL2 glew glm tinygltf)

# --- 4. Find System OpenGL ---
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE GRAPHICS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.cpp")

# --- 5. Project Executable ---
add_executable(${PROJECT_NAME} 
    ${CORE_SOURCES}
    ${GRAPHICS_SOURCES}
)

# --- 6. The "Magic" Path Fix ---
# Since glew-cmake downloads the real headers into the BUILD directory,
# we tell CMake to look in both the source and the binary output folders.
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${glew_SOURCE_DIR}/include
    ${glew_BINARY_DIR}/include
    ${sdl2_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${tinygltf_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics
)

# --- 7. Linking ---
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2-static
    SDL2::SDL2main
    libglew_static
    ${OPENGL_LIBRARIES}
)

# --- 8. MSVC Specifics ---
if(MSVC)
    add_compile_options(/arch:AVX2) #make sure SIMD optimizations take place
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
