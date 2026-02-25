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

    SDL_Window* window = SDL_CreateWindow("Modern OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();

    resources_init(argv[0]);

    // 1. Create VAO and VBO
    float vertices[] = {
        // verts             // colours          // texture coordinates
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute 1 : Position Data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attribute 2 : Colour Data
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Attribute 3 : Texture Coordinate
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 2. Compile Shaders (Simplified for this example)
    Shader myshader = Shader("shaders/shader.vs", "shaders/shader.fs");
    ImageData mytextureimage = resources_load_image("assets/palettes/island-joy-16-4x4.png");

    std::cout << "Image Width : " << mytextureimage.width << std::endl; 
    std::cout << "Image Height : " << mytextureimage.height << std::endl;
    std::cout << "Image Channels : " << mytextureimage.channels << std::endl;
    std::cout << "Image Data Read : " << mytextureimage.pixels[0];

    uint32_t textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Repeat on S axis, mirrored repeat on T axis...
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    // always use Nearest pixel
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mytextureimage.width, mytextureimage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mytextureimage.pixels);
    glGenerateMipmap(GL_TEXTURE_2D);


    // free the image data now we've loaded it!
    stbi_image_free(mytextureimage.pixels);

    // 3. Main Loop
    bool running = true;
    while (running) 
    {
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
                    
                    default:
                        break;
                }
            }            
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Setup a transformation matrix
        glm::mat4 trans = glm::mat4(1.0f); // 4x4 identity matrix
        //trans = glm::rotate(trans, (float)SDL_GetTicks64() / 1000.0f, glm::vec3(0.0f, 0.0f, 1.0f));

        // Setup a translation matrix
        glm::mat4 translation = glm::mat4(1.0f); // 4x4 identity matrix
        translation = glm::translate(translation, glm::vec3(glm::sin((float)SDL_GetTicks64() / 1000.0f) * 0.3, 0.0f, 0.0f));

        // Send it to the shader
        myshader.setMat4("transform", trans);
        myshader.setMat4("tanslate", translation);

        myshader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN,0, 4);

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
