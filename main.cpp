#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Camera.h"
#include <windows.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "Model.h"



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window,double xPos,double yPos);
void scroll_callback(GLFWwindow* window, double xOff,double yOff);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 1000;
unsigned int SCR_HEIGHT = 1000;
Camera ourCamera(SCR_WIDTH,SCR_HEIGHT);

int main()
{
    // 切换到英语键盘
    // -----------------------------------
    HKL hCurKL;
    hCurKL = GetKeyboardLayout(0);
    LoadKeyboardLayoutA(reinterpret_cast<LPCSTR>((LPCWSTR) ("0x0409")), KLF_ACTIVATE);

    float vScale = 1.0f;
    float my_color[4] = {1.0f};

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simple3DViewer", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetScrollCallback(window,scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // y-axis
    stbi_set_flip_vertically_on_load(true);
    // 开启z-buffer
    // --------------------------
    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui context
    // -----------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    // light
    // ---------------------------------------------------------------
    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f
    };



    // light
    // --------------------------------------------
    unsigned int VBO, lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 给缓冲区加值
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    // 好像是解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // light pos
    // -----------------------------------------------
    glm::vec3 lightPos(5.0f, 10.0f, 20.0f);
    float lightPos_x = lightPos.x;
    float lightPos_y = lightPos.y;

    // import shader and model
    //--------------------------------------------------------------------------------
    Shader ourShader("../Shader/shader.vs","../Shader/shader.fs");
    Shader lightShader("../Shader/light.vs","../Shader/light.fs");
    Model ourModel("../model/keli/keli.pmx");

    std::cout<<"before while"<<std::endl;
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        ourCamera.listenKeyboardEvent(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Start the Dear ImGui frame
        // -------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ourShader.use();
        glm::mat4 mScale = glm::mat4(1.0f);
        ImGui::Begin("Simple3DViewer");
        ImGui::Text("console");
        ImGui::SliderFloat("Scale Model", &vScale, 0.0f, 1.0f);
        ImGui::SliderFloat("lightPos_x",&lightPos_x,-10.0f,10.0f);
        ImGui::SliderFloat("lightPos_y",&lightPos_y,-10.0f,10.0f);
        ImGui::End();
        ImGui::Render();

        // set light pos
        // ---------------------------
        lightPos.x = lightPos_x;
        lightPos.y = lightPos_y;

        // 绘制图形
        // ------------------------------------------------------------------------
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        ourShader.setMat4("model", model);

        mScale = glm::scale(mScale,glm::vec3(vScale));
        ourShader.setMat4("scale",mScale);
        ourShader.setInt("material.specular", 1);
        ourShader.setFloat("material.shininess", 164.0f);
        ourShader.setVec3("light.ambient",  0.5f, 0.5f, 0.5f);
        ourShader.setVec3("light.diffuse",  0.7f, 0.7f, 0.7f); // 将光照调暗了一些以搭配场景
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("light.position",lightPos);
        ourShader.setVec3("light.direction", 0.0f,0.0f,-1.0f);
        ourShader.setFloat("light.cutOff",glm::cos(glm::radians(45.5f)));
        ourShader.setFloat("light.outerCutOff", glm::cos(glm::radians(60.5f)));
        ourShader.setVec3("viewPos",ourCamera.cameraPos);
        ourShader.setFloat("light.constant",  1.0f);
        ourShader.setFloat("light.linear",    0.022f);
        ourShader.setFloat("light.quadratic", 0.0019f);
        ourShader.setMat4("projection",ourCamera.getProjectionMat4());
        ourShader.setMat4("view",ourCamera.getViewMat4());
        ourModel.Draw(ourShader);

        // 绘制发光体
        // -----------------------------------------------------------
        lightShader.use();
        glBindVertexArray(lightVAO);
        lightShader.setMat4("projection",ourCamera.getProjectionMat4());
        lightShader.setMat4("view",ourCamera.getViewMat4());
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel,lightPos);
        lightModel = glm::scale(lightModel,glm::vec3(1.0f));
        lightShader.setMat4("model",lightModel);
        glDrawArrays(GL_TRIANGLES,0,36);
        glBindVertexArray(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwSwapInterval(1);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwDestroyWindow(window);
    glfwTerminate();

    // 将键盘切换回来
    // --------------------
    ActivateKeyboardLayout(hCurKL, 0);
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window,GLFW_KEY_LEFT_ALT) == GLFW_PRESS){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
    }
    if(glfwGetKey(window,GLFW_KEY_LEFT_ALT) == GLFW_RELEASE){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
    }



}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    SCR_HEIGHT = height;
    SCR_WIDTH = width;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window,double xPos,double yPos){
     ourCamera.mouseEvent(window,xPos,yPos);
}

void scroll_callback(GLFWwindow* window, double xOff,double yOff){
    ourCamera.scrollEvent(window,xOff,yOff,SCR_WIDTH,SCR_HEIGHT);
}