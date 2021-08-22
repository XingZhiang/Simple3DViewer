/*
 * Camera类
 * 相机类
 */
#pragma once
#ifndef VIEWER_CAMERA_H
#define VIEWER_CAMERA_H
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include "Shader.h"

// 一些相机参数
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

class Camera{
public:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
private:
    float deltaTime ; // 当前帧与上一帧的时间差
    float lastFrame ; // 上一帧的时间
    float lastX;  // 上一时刻鼠标的x位置
    float lastY;  // 上一时刻鼠标的y位置
    float Yaw;  // 偏航角
    float Pitch; // 俯仰角
    float Zoom; // 缩放大小
    unsigned int sWidth; // 屏幕宽度
    unsigned int sHeight;
public:
    glm::mat4 view;
    glm::mat4 projection;
public:
    explicit Camera(unsigned int screenWidth,unsigned int screenHeight):
        cameraPos(glm::vec3(0.0f, 0.0f,  3.0f)),
        cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
        cameraUp(glm::vec3(0.0f, 1.0f,  0.0f)),
        deltaTime(0.0f),
        lastFrame(0.0f),
        lastX(float(screenWidth)/2),
        lastY(float(screenHeight)/2),
        Yaw(YAW),
        Pitch(PITCH),
        Zoom(ZOOM),
        sWidth(screenWidth),
        sHeight(screenHeight),
        view(glm::mat4(1.0f)),
        projection(glm::perspective(glm::radians(45.0f),float(screenWidth)/float(screenHeight),0.1f,100.0f))
        {}

    glm::mat4 getViewMat4() const{
        return view;
    }
    glm::mat4 getProjectionMat4() const{
        return projection;
    }

    void listenKeyboardEvent(GLFWwindow* window){
        auto currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float cameraSpeed = 2.5f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraUp;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraUp;
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
            reset(window);
            return;
        }
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    void mouseEvent(GLFWwindow* window, double xPos, double yPos){
        float xOffset = float(xPos) - lastX;
        float yOffset = float(yPos) - lastY;
        lastX = float(xPos); lastY = float(yPos);
        float sensitivity = 0.005f;  // 鼠标灵敏度
        xOffset *= sensitivity; yOffset *= sensitivity;
        Yaw += xOffset; Pitch += yOffset;
        // 设置极限视角
        if(Pitch > 89.0f) Pitch = 89.0f;
        if(Pitch < -89.0f) Pitch = -89.0f;

        glm::vec3 front;
        front.x = float(cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)));
        front.y = float(sin(glm::radians(Pitch)));
        front.z = float(sin(glm::radians(Yaw)) * cos(glm::radians(Pitch)));
        cameraFront = glm::normalize(front);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }
    void scrollEvent(GLFWwindow* window, double xOff,double yOff, unsigned int screenWidth, unsigned int screenHeight){
        if(Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= float(yOff);
        if(Zoom <= 1.0f)
            Zoom = 1.0f;
        if(Zoom >= 45.0f)
            Zoom = 45.0f;
        projection = glm::perspective(glm::radians(Zoom),  float(screenWidth)/float(screenHeight), 0.1f, 100.0f);
    }

private:
    void reset(GLFWwindow* window){
        cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
        cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
        Zoom = ZOOM;
        glfwSetCursorPos(window, 0.0f, 0.0f);
        mouseEvent(window,0.0f,0.0f);
        scrollEvent(window,0.0f,0.0f,sWidth,sHeight);
    }
};


#endif //VIEWER_CAMERA_H
