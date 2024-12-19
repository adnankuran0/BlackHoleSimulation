#include "GL/glew.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int main(void)
{
    GLFWwindow* window;

    
    glm::vec2 resolution(SCR_WIDTH, SCR_HEIGHT);
    glm::vec4 mouse(0.0f, 0.0f, 0.0f, 0.0f);
    double mouseX, mouseY;

    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Black Hole Simulation", NULL, NULL);


    int AA = 2;
    float size = 0.3f;
    bool showDisk = true;
    float speed = 3.0f;
    float steps = 12.0f;
    bool fixedCamera = true;
    float distance = 1024.0f;
    float rotation = 355.0f;



    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW initialization ERROR!" << std::endl;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsClassic();
    ImGui_ImplOpenGL3_Init((char*)glGetString(330));

    std::cout << glGetString(GL_VERSION) << std::endl;


    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f,   // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
        -1.0f, 1.0f, 0.0f,    0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = { 
        0, 1, 3,  
        1, 2, 3   
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


    Shader shader("Source/shaders/vertex.vert", "Source/shaders/fragment.frag");
    
    shader.setInt("albedo", 0);


    stbi_set_flip_vertically_on_load(true);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Source/nebula.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);



        
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setVec2("resolution", resolution);
        shader.setFloat("time",glfwGetTime());
        shader.setVec2("resolution", resolution);
        glBindTexture(GL_TEXTURE_2D, texture);
        glfwGetCursorPos(window, &mouseX, &mouseY);
        mouse[0] = !fixedCamera ? (float)mouseX : distance;
        mouse[1] = !fixedCamera ? (float)mouseY : rotation;


        mouse[2] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) ? 1.0f : 0.0f;
        mouse[3] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) ? 1.0f : 0.0f;
        shader.setVec4("mouse", mouse);
        shader.setFloat("speed", speed);
        shader.setInt("AA", AA);
        shader.setFloat("steps",steps);
        shader.setFloat("size", size);
        shader.setBool("disk", showDisk);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("BLACK HOLE SETTINGS", nullptr);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::SliderInt("Anti-Aliasing", &AA, 1, 8);
            ImGui::SliderFloat("Black Hole Size", &size, 0.1f, 1.0f);
            ImGui::SliderFloat("Black Hole Speed", &speed, 0.0f, 10.0f);
            ImGui::SliderFloat("Simulation Steps", &steps, 2.0f, 64.0f);
            ImGui::Checkbox("Show Disk", &showDisk);
            ImGui::Checkbox("Fixed Camera", &fixedCamera);
            ImGui::SliderFloat("Camera Rotation", &rotation, 0, SCR_HEIGHT);
            ImGui::SliderFloat("Camera Distance", &distance, 0, SCR_WIDTH);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}