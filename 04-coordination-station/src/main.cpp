#include <GL/glew.h>
#include <SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "mad_shader.h"
#include "resources.h"
#include "stb_image.h"

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    // Request OpenGL 3.3 Core Profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("Modern OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL ); // | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();

    resources_init(argv[0]);

    // 1. Create VAO and VBO
    float vertices[] =
    {
        // verts             // colours          // texture coordinates
        0.1f,  0.1f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.1f, -0.1f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.1f, -0.1f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.1f,  0.1f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };

    uint32_t indicies[] =
    {
        0,1,2, // first triangle
        0,2,3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);;

    // Attribute 1 : Position Data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attribute 2 : Colour Data
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Attribute 3 : Texture Coordinate
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 2. Compile Shaders
    Shader myshader = Shader("shaders/vert-shader.glsl", "shaders/frag-shader.glsl");
    ImageData mytextureimage = resources_load_image("assets/imgs/bx-side.png");

    std::cout << "Image Width : " << mytextureimage.width << std::endl;
    std::cout << "Image Height : " << mytextureimage.height << std::endl;
    std::cout << "Image Channels : " << mytextureimage.channels << std::endl;

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
    ImageData mypalette = resources_load_image("assets/palettes/shido-cyberneon-1x.png");
    std::cout << "Image Width : " << mypalette.width << std::endl;
    std::cout << "Image Height : " << mypalette.height << std::endl;
    std::cout << "Image Channels : " << mypalette.channels << std::endl;
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

    myshader.use();
    glUniform1i(glGetUniformLocation(myshader.ID, "tex1"), 0);
    myshader.setInt("tex2", 1);

    float mix_factor = 1.0f;

    glm::fvec2 position = glm::fvec2(0.0f, 0.0f);
    bool mv_keys[4] =
    {
        false,  // up
        false,  // down
        false,  // Left
        false   // right
    };

    glm::fvec2 direction = glm::fvec2(0.0f, 0.0f);
    float last = (float)SDL_GetTicks64();

    // 3. Main Loop
    bool running = true;
    while (running)
    {
        direction.x = 0;
        direction.y = 0;

        float now = (float)SDL_GetTicks64();
        mix_factor = 0.5f * (1.0f + glm::cos(now / 2000));

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

                    case SDLK_SPACE:
                        position.x = 0.0f;
                        position.y = 0.0f;
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

                    default:
                        break;
                }
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
        if(mv_keys[2])
        {
            direction.x -= 1.0;
        }
        if(mv_keys[3])
        {
            direction.x += 1.0;
        }

        // std::cout << "DIR := {" << direction.x << "," << direction.y << "}" << std::endl;

        position.x += direction.x * (now - last)/1000 * 0.5f;
        position.y += direction.y * (now - last)/1000 * 0.5f;
        // std::cout << "POS := {" << position.x << "," << position.y << "}" << std::endl;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Bind textures to texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, paletteID);

        // Setup a transformation matrix
        glm::mat4 trans = glm::mat4(1.0f); // 4x4 identity matrix
        trans = glm::translate(trans, glm::vec3(position.x, position.y, 0.0f));
        trans = glm::rotate(trans, (float)SDL_GetTicks64() / 500.0f, glm::vec3(0.0f, 0.0f, 1.0f));

        // Send it to the shader
        myshader.setMat4("transform", trans);
        myshader.setFloat("factor", mix_factor);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
        last = now;
    }

    return 0;
}
