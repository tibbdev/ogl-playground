#include <GL/glew.h>
#include <SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

// Simple Shaders (written in GLSL)
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    
    uniform mat4 transform;
    out vec4 vertexColour;

    void main() 
    {
        gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
        vertexColour = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec4 vertexColour;
    void main() 
    {
        FragColor = vertexColour;
    }
)";

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

    // 1. Create VAO and VBO
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 2. Compile Shaders (Simplified for this example)
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

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
        trans = glm::rotate(trans, (float)SDL_GetTicks64() / 1000.0f, glm::vec3(0.0f, 0.0f, 1.0f));

        // Send it to the shader
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
