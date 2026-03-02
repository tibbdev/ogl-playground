cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(coordination-ogl LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- 1. Global Build Settings (Forces everything to be Static) ---
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# SDL2 Settings
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

# SDL_mixer Settings - Force Static and disable dependencies
set(SDL2MIXER_VENDORED ON CACHE BOOL "" FORCE)
set(SDL2MIXER_DEPS_SHARED OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_FLAC OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MIDI OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MOD OFF CACHE BOOL "" FORCE)

# PhysFS Settings
set(PHYSFS_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_STATIC ON CACHE BOOL "" FORCE)

# --- 2. Fetching ---
FetchContent_Declare(SDL2 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG release-2.30.0)
FetchContent_Declare(SDL_mixer GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git GIT_TAG release-2.8.0)
FetchContent_Declare(glew GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git GIT_TAG glew-cmake-2.3.1)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
FetchContent_Declare(tinygltf GIT_REPOSITORY https://github.com/syoyo/tinygltf.git GIT_TAG v2.9.3)
FetchContent_Declare(physfs GIT_REPOSITORY https://github.com/icculus/physfs.git GIT_TAG release-3.2.0)

FetchContent_MakeAvailable(SDL2 SDL_mixer glew glm tinygltf physfs)

find_package(OpenGL REQUIRED)

# --- 3. Source Management ---
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE GRAPHICS_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/graphics/*.cpp")

# if (NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
# endif()
# message("Build Type : ${CMAKE_BUILD_TYPE}")

add_executable(${PROJECT_NAME} ${CORE_SOURCES} ${GRAPHICS_SOURCES})

# --- 4. Compiler Settings ---
if(MSVC)
    add_compile_options(/arch:AVX2)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# --- 5. Include Directories ---
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${glew_SOURCE_DIR}/include
    ${glew_BINARY_DIR}/include
    ${sdl2_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${tinygltf_SOURCE_DIR}
    ${physfs_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics
)

# --- 6. Linking ---
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2-static
    SDL2::SDL2main
    SDL2_mixer
    libglew_static
    physfs-static
    ${OPENGL_LIBRARIES}
)

# --- 7. Resource Management ---

# --- 7. Resource Management ---
# --- 7. Resource Management ---

# DEVELOPMENT: Always sync folders
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Syncing assets for development..."
)

# RELEASE PACKING: Uses a Generator Expression to only run in Release
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # This command only runs if the configuration is NOT Debug
    COMMAND $<$<NOT:$<CONFIG:Debug>>:${CMAKE_COMMAND}> $<$<NOT:$<CONFIG:Debug>>:-E> $<$<NOT:$<CONFIG:Debug>>:tar> $<$<NOT:$<CONFIG:Debug>>:cf> $<$<NOT:$<CONFIG:Debug>>:data.zip> $<$<NOT:$<CONFIG:Debug>>:--format=zip> "shaders" "assets"
    
    # Append to Binary (Windows) - Only runs in non-Debug
    COMMAND $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:cmd> $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:/c> "copy /b $<TARGET_FILE_NAME:${PROJECT_NAME}> + data.zip temp.bin && move /y temp.bin $<TARGET_FILE_NAME:${PROJECT_NAME}>"
    
    # Append to Binary (Linux) - Only runs in non-Debug
    COMMAND $<$<AND:$<PLATFORM_ID:Linux>,$<NOT:$<CONFIG:Debug>>>:sh> $<$<AND:$<PLATFORM_ID:Linux>,$<NOT:$<CONFIG:Debug>>>:-c> "cat $<TARGET_FILE_NAME:${PROJECT_NAME}> data.zip > temp.bin && mv temp.bin $<TARGET_FILE_NAME:${PROJECT_NAME}> && chmod +x $<TARGET_FILE_NAME:${PROJECT_NAME}>"
    
    # Cleanup
    # COMMAND $<$<NOT:$<CONFIG:Debug>>:${CMAKE_COMMAND}> $<$<NOT:$<CONFIG:Debug>>:-E> $<$<NOT:$<CONFIG:Debug>>:remove> "data.zip"
    
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Packing assets (Release only)..."
)
