#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraType
{
    BasicFly,
    FPS
};

struct Camera
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float     yaw = -90.0f;
    float     pitch = 0.0f;
    //float     roll = 0.0f;
    float     fov = 60.0f;
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
    CameraType type = CameraType::BasicFly;

    glm::mat4 update(float deltaTime, glm::fvec2 mv_dir, glm::ivec2 deltaMouse, float speed, float sensitivity, int8_t scroll_dir)
    {
        yaw += (float)deltaMouse.x * sensitivity;
        pitch -= (float)deltaMouse.y * sensitivity;

        if (60.0f < pitch)
        {
            pitch = 60.0f;
        }
        if (-60.0f > pitch)
        {
            pitch = -60.0f;
        }

        direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
        direction.y = glm::sin(glm::radians(pitch));
        direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

        front = glm::normalize(direction);

        switch (type)
        {
            case CameraType::BasicFly:
                position += mv_dir.y * speed * deltaTime * front;
                position += glm::normalize(glm::cross(front, up)) * speed * deltaTime * mv_dir.x;

                return glm::lookAt(position, position + front, up);

            case CameraType::FPS:
                glm::vec3 front_temp = front;

                front_temp.y = 0.0f;

                position += mv_dir.y * speed * deltaTime * front_temp;
                position += glm::normalize(glm::cross(front_temp, up)) * speed * deltaTime * mv_dir.x;

                return glm::lookAt(position, position + front, up);
                break;
            default:
                break;
        }
    }
};
