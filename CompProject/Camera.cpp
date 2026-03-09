#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 startPos)
{
    Position = startPos;

    WorldUp = glm::vec3(0, 1, 0);

    Yaw = -90.0f;
    Pitch = 0.0f;

    Speed = 0.1f;
    Sensitivity = 0.1f;

    updateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::processKeyboard(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Position += Speed * Front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Position -= Speed * Front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Position -= glm::normalize(glm::cross(Front, Up)) * Speed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Position += glm::normalize(glm::cross(Front, Up)) * Speed;
}

void Camera::processMouse(GLFWwindow* window)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    xoffset *= Sensitivity;
    yoffset *= Sensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    updateVectors();
}

void Camera::updateVectors()
{
    glm::vec3 front;

    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}