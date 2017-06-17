#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
    this->Position = position;
    this->WorldUp = up;
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

glm::mat4 Camera::getPerspectiveMatrix() {
    return glm::perspective(glm::radians(this->Zoom), 800.0f/600.0f, 0.1f, 1000.0f);
}

void Camera::onKeyboard(Camera_Movement direction, GLfloat deltaTime) {
    GLfloat velocity = this->MovementSpeed * deltaTime;
    if (direction == FORWARD) {
        this->Position += this->Front * velocity;
    }
    if (direction == BACKWARD) {
        this->Position -= this->Front * velocity;
    }
    if (direction == LEFT) {
        this->Position -= this->Right * velocity;
    }
    if (direction == RIGHT) {
        this->Position += this->Right * velocity;
    }
}

void Camera::onMouseMove(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)  {
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    this->Yaw   += xoffset;
    this->Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (this->Pitch > 89.0f) {
            this->Pitch = 89.0f;
        }
        if (this->Pitch < -89.0f) {
            this->Pitch = -89.0f;
        }
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    this->updateCameraVectors();
}

void Camera::onMouseScroll(GLfloat yoffset)     {
    if (this->Zoom >= 1.0f && this->Zoom <= 45.0f) {
        this->Zoom -= yoffset / 100;
    }
    if (this->Zoom <= 1.0f) {
        this->Zoom = 1.0f;
    }
    if (this->Zoom >= 45.0f) {
        this->Zoom = 45.0f;
    }
}

void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = glm::cos(glm::radians(this->Yaw)) * glm::cos(glm::radians(this->Pitch));
    front.y = glm::sin(glm::radians(this->Pitch));
    front.z = glm::sin(glm::radians(this->Yaw)) * glm::cos(glm::radians(this->Pitch));
    this->Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
    this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
}
