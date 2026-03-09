#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:

    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float Speed;
    float Sensitivity;

    bool firstMouse = true;
    float lastX = 0.0f;
    float lastY = 0.0f;

    Camera(glm::vec3 startPos);

    void processKeyboard(GLFWwindow* window);
    void processMouse(GLFWwindow* window);

    glm::mat4 getViewMatrix();

private:
    void updateVectors();
};