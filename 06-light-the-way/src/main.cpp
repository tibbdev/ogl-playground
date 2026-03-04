/**
  *  Learning OpenGL stuff! 
  */
#include <GL/glew.h>
#include <SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "mad_shader.h"
#include "mad_camera.h"
#include "resources.h"
#include "stb_image.h"

constexpr float floor_x = 128;
constexpr float floor_y = 128;
constexpr float DEFAULT_WINDOW_H = 1080.0f;
constexpr float DEFAULT_WINDOW_W = 1920.0f;
constexpr float DEFAULT_MOUSE_SENSITIVITY = 0.01f;
constexpr glm::vec3 cubePositions[] =
{
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};
constexpr float cubeScales[] =
{
    0.18f, 0.5f, 0.25f, 0.4f, 0.6f, 0.3f, 0.2f, 0.15f, 0.45f, 0.33f
};
constexpr glm::vec3 lightPosition = glm::vec3(2.4f, 1.5f, 5.0f);

struct Settings
{
    float height = DEFAULT_WINDOW_H;
    float width = DEFAULT_WINDOW_W;
    std::string title = "Modern OpenGL :: LearnOpenGL - Lighting...";

    float fov = 45.0f;
    float sensitivity = DEFAULT_MOUSE_SENSITIVITY;
    Camera cam;
};

glm::vec3 cubedfloor[(uint16_t)(floor_x * floor_y)];

Settings g_settings;

glm::fvec2 direction = glm::fvec2(0.0f, 0.0f);

static uint32_t bind_gltf_model(ModelData& mdata)
{
    if (mdata.model.meshes.empty())
    {
        std::cout << "bind_gltf_model :: Model is Empty, nothing to bind..." << std::endl;
        return -1;
    }

    if (mdata.model.meshes.front().primitives.empty())
    {
        std::cout << "bind_gltf_model :: Mesh-0 primitives is Empty, nothing to bind..." << std::endl;
        return -1;
    }

    // Assume only one mesh per model and one primitive per mesh
    const tinygltf::Primitive& primitive = mdata.model.meshes.front().primitives.front();

    // Create Vertex Array
    glGenVertexArrays(1, &mdata.vao);
    glBindVertexArray(mdata.vao);

    // Loop through and process Attributes
    for (auto& attrib : primitive.attributes)
    {
        // Get info from the model
        const tinygltf::Accessor& accessor      = mdata.model.accessors[attrib.second];
        const tinygltf::BufferView& buffView    = mdata.model.bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buff            = mdata.model.buffers[buffView.buffer];

        // std::cout << "Processing attrib : " << attrib.first << std::endl;

        glGenBuffers(1, &mdata.vbo);              // Create Vertex Buffer
        glBindBuffer(GL_ARRAY_BUFFER, mdata.vbo); // Bind the buffer
        // Load data to the buffer
        glBufferData(GL_ARRAY_BUFFER, buffView.byteLength, &buff.data[buffView.byteOffset], GL_STATIC_DRAW); 

        // Map Vertex Attribute to shader location
        int location = -1;
        
        if (attrib.first == "POSITION")
        {
            location = 0;
        }
        else if (attrib.first == "NORMAL")
        {
            location = 1;
        }
        else if (attrib.first == "TEXCOORD_0")
        {
            location = 2;
        }
        else
        {
            continue;
        }

        if (location != -1)
        {
            glVertexAttribPointer(location, accessor.type, accessor.componentType, GL_FALSE, buffView.byteStride, (void*)accessor.byteOffset);
            glEnableVertexAttribArray(location);
        }
    }

    if (primitive.indices > -1)
    {
        // Get the Indicies Data if it's there!
        const tinygltf::Accessor indxAcc         = mdata.model.accessors[primitive.indices];
        const tinygltf::BufferView& indxBuffView = mdata.model.bufferViews[indxAcc.bufferView];
        const tinygltf::Buffer& indxBuff         = mdata.model.buffers[indxBuffView.buffer];

        mdata.indx_cnt  = indxAcc.count;
        mdata.type      = indxAcc.componentType;

        glGenBuffers(1, &mdata.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdata.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxBuffView.byteLength, &indxBuff.data[indxBuffView.byteOffset], GL_STATIC_DRAW);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    // Request OpenGL 4.3 Core Profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow(g_settings.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, g_settings.width, g_settings.height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);

    resources_init(argv[0]);

    ModelData my3dmodel = resources_load_model_glb("assets/models/better-box.glb");
    uint32_t model_bind_result = bind_gltf_model(my3dmodel);

    if (0 != model_bind_result)
    {
        return model_bind_result;
    }

    ModelData basicCube = resources_load_model_glb("assets/models/box.glb");
    model_bind_result = bind_gltf_model(basicCube);
    if (0 != model_bind_result)
    {
        return model_bind_result;
    }

    // 2. Compile Shaders
    Shader lightingShader       = Shader("shaders/vert-model.glsl", "shaders/frag-lighting.glsl");
    Shader lightSourceShader    = Shader("shaders/vert-model.glsl", "shaders/frag-light.glsl");
    Shader colourShader         = Shader("shaders/vert-model.glsl", "shaders/frag-colour.glsl");

    ImageData mytextureimage = resources_load_image("assets/palettes/sunset-red-8x.png");
    //ImageData mytextureimage = resources_load_image("assets/palettes/shoshone-5-8x.png");
        
    // Setup first texture
    uint32_t textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Repeat on S axis, mirrored repeat on T axis...
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // always use Nearest pixel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mytextureimage.width, mytextureimage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mytextureimage.pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    // free the image data now we've loaded it!
    stbi_image_free(mytextureimage.pixels);

    // Setup second texture
    //ImageData mypalette = resources_load_image("assets/palettes/shido-cyberneon-1x.png"););
    ImageData mypalette = resources_load_image("assets/palettes/shoshone-5-8x.png");

    uint32_t paletteID;
    glGenTextures(1, &paletteID);
    glBindTexture(GL_TEXTURE_2D, paletteID);

    // Repeat on S axis, mirrored repeat on T axis...
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // always use Nearest pixel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mypalette.width, mypalette.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mypalette.pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    // free the image data now we've loaded it!
    stbi_image_free(mypalette.pixels);

    for (float y = 0; floor_y > y; y += 1.0f)
    {
        for (float x = 0; floor_x > x; x += 1.0f)
        {
            cubedfloor[(uint16_t)(x + y * floor_x)] = 
            glm::vec3(
                (x - (floor_x * 0.5f)) * 0.5f, 
                -3.5f, 
                0.5f * (y - floor_y * 0.5f)
            );
        }
    }

    float mix_factor = 1.0f;

    bool mv_keys[4] =
    {
        false,  // up
        false,  // down
        false,  // Left
        false   // right
    };

    float now = (float)SDL_GetTicks64();
    float last = now;
    float radius = 5.0f;

    glm::mat4 view;

    uint8_t project_select = 0; // 0 == perspective, 1 = ortho

    glm::ivec2 mouseNow = glm::ivec2(0);

    uint32_t mouseButtons = SDL_GetMouseState(&mouseNow.x, &mouseNow.y);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    bool crouching = false;

    // 3. Main Loop
    bool running = true;
    while (running)
    {
        now = (float)SDL_GetTicks64();
        float cameraSpeed = 0.01f;
        float deltaTime = now - last;

        direction.x = 0;
        direction.y = 0;

        mix_factor = 0.5f * (1.0f + glm::cos(now / 1000));

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }

            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;

                case SDLK_w:
                case SDLK_UP:
                    mv_keys[0] = true;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    mv_keys[1] = true;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    mv_keys[2] = true;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    mv_keys[3] = true;
                    break;

                case SDLK_r:
                    g_settings.fov = 45.0f;
                    break;

                case SDLK_SPACE:
                    g_settings.cam.position.x = 0.0f;
                    g_settings.cam.position.z = 0.0f;
                    break;

                case SDLK_p:
                    project_select = 0;
                    break;

                case SDLK_o:
                    project_select = 1;
                    break;

                case SDLK_LCTRL:
                    crouching = true;
                    break;

                case SDLK_1:
                    g_settings.cam.type = CameraType::BasicFly;
                    break;
                case SDLK_2:
                    g_settings.cam.type = CameraType::FPS;
                    break;

                default:
                    break;
                }
            }

            if (e.type == SDL_KEYUP)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_w:
                case SDLK_UP:
                    mv_keys[0] = false;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    mv_keys[1] = false;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    mv_keys[2] = false;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    mv_keys[3] = false;
                    break;

                case SDLK_LCTRL:
                    crouching = false;
                    break;

                default:
                    break;
                }
            }

            if (e.type == SDL_MOUSEWHEEL)
            {
                if (e.wheel.y > 0.1f)
                {
                    if (g_settings.fov < 90.0f)
                    {
                        g_settings.fov += 0.5f;
                    }
                }
                else if (e.wheel.y < -0.1f)
                {
                    if (g_settings.fov > 5.0f)
                    {
                        g_settings.fov -= 0.5f;
                    }
                }
                //std::cout << "fov := " << g_settings.fov << std::endl;
            }
        }

        if(mv_keys[0])
        {
            direction.y += 1.0;
        }
        if(mv_keys[1])
        {
            direction.y -= 1.0;
        }
        if(mv_keys[2]) // Strafe Left
        {
            direction.x -= 1.0;
        }
        if(mv_keys[3]) // Strafe Right
        {
            direction.x += 1.0;
        }

        mouseButtons = SDL_GetRelativeMouseState(&mouseNow.x, &mouseNow.y);

        glClearColor(0.1f, 0.05f, 0.15f * mix_factor, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind textures to texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, paletteID);

        // setup a projection matrix
        glm::mat4 project = glm::perspective(glm::radians(g_settings.fov), g_settings.width / g_settings.height, 0.1f, 100.0f);

        view = g_settings.cam.update(deltaTime, direction, mouseNow, cameraSpeed, g_settings.sensitivity, 0.0f);


        lightingShader.use();
        lightingShader.setInt("tex1", 0);
        lightingShader.setInt("tex2", 1);

        // Send it to the shader
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", project);
        lightingShader.setVec3("lightColour", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setVec3("lightPos", glm::vec3(lightPosition));

        glBindVertexArray(my3dmodel.vao);

        for (size_t idx = 0; idx < (sizeof(cubePositions) / sizeof(cubePositions[0])); idx++)
        {
            // Setup a transformation matrix
            glm::mat4 model = glm::mat4(1.0f); // 4x4 identity matrix
            model = glm::translate(model, cubePositions[idx]);

            // Bounce first cube
            if(idx == 0)
            {
                float sin_time = glm::sin(glm::radians(now/50));
                model = glm::translate(model, glm::vec3(0.0f, sin_time * sin_time, 0.0f));
            }

            float angle = 20.0f * idx;

            model = glm::scale(model, glm::vec3(cubeScales[idx]));
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 3.0f, 5.0f));
            
            if (0 == (idx % 3))
            {
                model = glm::rotate(model, glm::radians(now / 1000.0f), glm::vec3(0.5f, 0.5f, 1.0f));
            }

            lightingShader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, my3dmodel.indx_cnt, my3dmodel.type, 0);
        }

        glBindVertexArray(basicCube.vao);

        for (size_t idx = 0; idx < (sizeof(cubedfloor) / sizeof(cubedfloor[0])); idx++)
        {
            // Setup a transformation matrix
            glm::mat4 model = glm::mat4(1.0f); // 4x4 identity matrix
            model = glm::translate(model, cubedfloor[idx]);
            model = glm::scale(model, glm::vec3(0.1f));

            lightingShader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, basicCube.indx_cnt, basicCube.type, 0);
        }

        lightSourceShader.use();
        
        // Set up the view and projection matricies for the light source
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("projection", project);

        // Setup a transformation matrix
        glm::mat4 model = glm::mat4(1.0f); // 4x4 identity matrix
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.1f));

        lightSourceShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, basicCube.indx_cnt, basicCube.type, 0);

        SDL_GL_SwapWindow(window);
        last = now;
    }

    return 0;
}
