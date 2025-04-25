#include <cmath>
#include <cstdio>
#include <string>
#include <glad/glad.h>
#include <glfw3.h>
#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "glm/trigonometric.hpp"
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "nmath.h"

// TODO:@ntipper ^ Write my own image importer as a fun project!

typedef unsigned int uint;
#define global_variable static
#define CAMERA_SPEED 2.5f
global_variable glm::vec3 cameraPos;
global_variable glm::vec3 cameraFront;
global_variable glm::vec3 cameraUp;
global_variable float deltaTime;
global_variable float lastFrame;
global_variable bool firstMouse;
global_variable float lastX;
global_variable float lastY;
global_variable float yaw = -90.0f;
global_variable float pitch = 0.0f;
global_variable float fov = 45.0f;

static bool
MakeHex(glm::vec3 center, float sideLength, float* outVertexList)
{
    if(!outVertexList)
    {
        return false;
    }

    float orientation = 30.0f;
    int currentVertex = 0;
    glm::vec3 startingPosition = { center.x + sideLength, center.y, center.z };
    glm::mat3 rotationMatrix = glm::mat3(1.0f);

    for(int i = 1; i <= 6; ++i)
    {
        float vertexRotation = orientation + (60.0f * (i - 1));
        rotationMatrix[0][0] = cos(glm::radians(vertexRotation));
        rotationMatrix[0][2] = -sin(glm::radians(vertexRotation));
        rotationMatrix[2][0] = sin(glm::radians(vertexRotation));
        rotationMatrix[2][2] = cos(glm::radians(vertexRotation));

        glm::vec3 vertexPosition = rotationMatrix * startingPosition;
        outVertexList[currentVertex++] = vertexPosition.x;
        outVertexList[currentVertex++] = vertexPosition.y;
        outVertexList[currentVertex++] = vertexPosition.z;
    }

    return false;
}

uint LoadImage(char* filename)
{
    uint Result = 0;

    glGenTextures(1, &Result);
    glBindTexture(GL_TEXTURE_2D, Result);

    // stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if(data)
    {
        GLenum format;
        if(nrChannels == 1)
        {
            format = GL_RED;
        }
        else if(nrChannels == 3)
        {
            format = GL_RGB;
        }
        else if(nrChannels == 4)
        {
            format = GL_RGBA;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        printf("Could not load file data\n");
    }
    return Result;
}

void processInput(GLFWwindow* window)
{
    float cameraSpeed = CAMERA_SPEED * deltaTime;
    glm::vec3 projCameraFrontToXZ = cameraFront * glm::vec3(1.f, 0.f, 1.f);
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, 1);
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * projCameraFrontToXZ;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * projCameraFrontToXZ;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * glm::normalize(glm::cross(projCameraFrontToXZ, cameraUp));
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * glm::normalize(glm::cross(projCameraFrontToXZ, cameraUp));
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if(fov < 1.0f)
    {
        fov = 1.0f;
    }
    else if(fov > 45.0f)
    {
        fov = 45.0f;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

glm::mat4 lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up)
{
    glm::mat4 Result = glm::mat4(1.0f);

    glm::vec3 dir = glm::normalize(pos - target);
    glm::vec3 right = glm::normalize(glm::cross(glm::normalize(up), dir));
    glm::vec3 cameraUp = glm::cross(dir, right);

    glm::mat4 translation = glm::mat4(1.0f);
    translation[3][0] = -pos.x;
    translation[3][1] = -pos.y;
    translation[3][2] = -pos.z;
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0][0] = right.x;
    rotation[1][0] = right.y;
    rotation[2][0] = right.z;
    rotation[0][1] = cameraUp.x;
    rotation[1][1] = cameraUp.y;
    rotation[2][1] = cameraUp.z;
    rotation[0][2] = dir.x;
    rotation[1][2] = dir.y;
    rotation[2][2] = dir.z;

    Result = rotation * translation;
    return Result;
}

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // TODO: These globals we will need to get rid of at some point
    // Initialize global_variables
    cameraPos = glm::vec3(0.f, 0.f, 3.0f);
    cameraFront = glm::vec3(.0f, .0f, -1.f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    deltaTime = lastFrame = 0.f;
    glm::vec3 direction = glm::vec3(0.f, 0.f, 0.f);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    ShaderProgram sp0;
    shader_init(&sp0);
    shader_set_source(&sp0, SHADERTYPE_VERTEX, (char*)"..\\shaders\\shader.vert");
    shader_set_source(&sp0, SHADERTYPE_FRAGMENT, (char*)"..\\shaders\\shader.frag");
    shader_load(&sp0);
    shader_link(&sp0);

    ShaderProgram lightSourceShader;
    shader_init(&lightSourceShader);
    shader_set_source(&lightSourceShader, SHADERTYPE_VERTEX, (char*)"..\\shaders\\shader.vert");
    shader_set_source(&lightSourceShader, SHADERTYPE_FRAGMENT, (char*)"..\\shaders\\light.frag");
    shader_load(&lightSourceShader);
    shader_link(&lightSourceShader);

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f),
    };

    uint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // Light source
    uint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // LOAD TEXTURE
    unsigned int diffuseMap = LoadImage((char*)"..\\data\\container2.png");
    unsigned int specularMap = LoadImage((char*)"..\\data\\container2_specular.png");
    unsigned int emissionMap = LoadImage((char*)"..\\data\\matrix.jpg");

    // "Unbind"
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
    float currentFrame = 0.f;
    while(!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_use(&sp0);
        shader_set_int(&sp0, (char*)("material.diffuse"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        shader_set_int(&sp0, (char*)("material.specular"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        shader_set_int(&sp0, (char*)("material.emission"), 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMap);

        glm::mat4 proj = glm::mat4(1.f);

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
        proj = glm::perspective(glm::radians(fov), 800.f / 600.f , 0.1f, 100.f);
        glm::mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader_set_matrix(&sp0, (char*)("projection"), glm::value_ptr(proj));
        shader_set_matrix(&sp0, (char*)("view"), glm::value_ptr(view));
        shader_set_vec3(&sp0, (char*)("viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
        shader_set_float(&sp0, (char*)("material.shininess"), 64.0f);


        shader_set_vec3(&sp0, (char*)("dirLight.ambient"), .05f, .05f, .05f);
        shader_set_vec3(&sp0, (char*)("dirLight.diffuse"), .4f, .4f, .4f);
        shader_set_vec3(&sp0, (char*)("dirLight.specular"), .5f, .5f, .5f);
        shader_set_vec3(&sp0, (char*)("dirLight.direction"), -.2f, -1.0f, -.3f);

        // Spot Light
        shader_set_vec3(&sp0, (char*)("spotLight.direction"), cameraFront.x, cameraFront.y, cameraFront.z);
        shader_set_vec3(&sp0, (char*)("spotLight.position"), cameraPos.x, cameraPos.y, cameraPos.z);
        shader_set_float(&sp0, (char*)("spotLight.innerCone"), glm::cos(glm::radians(12.5f)));
        shader_set_float(&sp0, (char*)("spotLight.outerCone"), glm::cos(glm::radians(17.5f)));
        shader_set_vec3(&sp0, (char*)("spotLight.ambient"), .2f, .2f, .2f);
        shader_set_vec3(&sp0, (char*)("spotLight.diffuse"), .5f, .5f, .5f);
        shader_set_vec3(&sp0, (char*)("spotLight.specular"), 1.f, 1.f, 1.f);

        // Point Lights
        shader_set_vec3(&sp0, (char*)("pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        shader_set_vec3(&sp0, (char*)("pointLights[0].ambient"), 0.2f, 0.2f, 0.2f);
        shader_set_vec3(&sp0, (char*)("pointLights[0].diffuse"), 0.5f, 0.5f, 0.5f);
        shader_set_vec3(&sp0, (char*)("pointLights[0].ambient"), 1.f, 1.f, 1.f);
        shader_set_float(&sp0, (char*)("pointLights[0].constant"), 1.0f);
        shader_set_float(&sp0, (char*)("pointLights[0].linear"), .09f);
        shader_set_float(&sp0, (char*)("pointLights[0].quadratic"), .032f);
        
        shader_set_vec3(&sp0, (char*)("pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        shader_set_vec3(&sp0, (char*)("pointLights[1].ambient"), 0.2f, 0.2f, 0.2f);
        shader_set_vec3(&sp0, (char*)("pointLights[1].diffuse"), 0.5f, 0.5f, 0.5f);
        shader_set_vec3(&sp0, (char*)("pointLights[1].ambient"), 1.f, 1.f, 1.f);
        shader_set_float(&sp0, (char*)("pointLights[1].constant"), 1.0f);
        shader_set_float(&sp0, (char*)("pointLights[1].linear"), .09f);
        shader_set_float(&sp0, (char*)("pointLights[1].quadratic"), .032f);
        
        shader_set_vec3(&sp0, (char*)("pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        shader_set_vec3(&sp0, (char*)("pointLights[2].ambient"), 0.2f, 0.2f, 0.2f);
        shader_set_vec3(&sp0, (char*)("pointLights[2].diffuse"), 0.5f, 0.5f, 0.5f);
        shader_set_vec3(&sp0, (char*)("pointLights[2].ambient"), 1.f, 1.f, 1.f);
        shader_set_float(&sp0, (char*)("pointLights[2].constant"), 1.0f);
        shader_set_float(&sp0, (char*)("pointLights[2].linear"), .09f);
        shader_set_float(&sp0, (char*)("pointLights[2].quadratic"), .032f);
        
        shader_set_vec3(&sp0, (char*)("pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        shader_set_vec3(&sp0, (char*)("pointLights[3].ambient"), 0.2f, 0.2f, 0.2f);
        shader_set_vec3(&sp0, (char*)("pointLights[3].diffuse"), 0.5f, 0.5f, 0.5f);
        shader_set_vec3(&sp0, (char*)("pointLights[3].ambient"), 1.f, 1.f, 1.f);
        shader_set_float(&sp0, (char*)("pointLights[3].constant"), 1.0f);
        shader_set_float(&sp0, (char*)("pointLights[3].linear"), .09f);
        shader_set_float(&sp0, (char*)("pointLights[3].quadratic"), .032f);

#if 0
        for(int i = 0; i < 4; ++i)
        {
            std::string currentLightString = "pointLights[0]";
            currentLightString[12] = (char)i;
            shader_set_vec3(&sp0, const_cast<char*>((currentLightString + std::string(".ambient")).c_str()), .2f, .2f, .2f);
            shader_set_vec3(&sp0, const_cast<char*>((currentLightString + std::string(".diffuse")).c_str()), .5f, .5f, .5f);
            shader_set_vec3(&sp0, const_cast<char*>((currentLightString + std::string(".specular")).c_str()), 1.f, 1.f, 1.f);
            shader_set_float(&sp0, const_cast<char*>((currentLightString + std::string(".constant")).c_str()), 1.0f);
            shader_set_float(&sp0, const_cast<char*>((currentLightString + std::string(".linear")).c_str()), 0.09f);
            shader_set_float(&sp0, const_cast<char*>((currentLightString + std::string(".quadratic")).c_str()), 0.032f);
            shader_set_vec3(&sp0, const_cast<char*>((currentLightString + std::string(".position")).c_str()), pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
        }
#endif
        glBindVertexArray(VAO);

        for(int i = 0; i < 10; ++i)
        {
            glm::mat4 model = glm::mat4(1.f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader_set_matrix(&sp0, (char*)"model", glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for(int i = 0; i < 4; ++i)
        {
            glm::mat4 model = glm::mat4(1.f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));

            shader_use(&lightSourceShader);
            shader_set_matrix(&lightSourceShader, (char*)("projection"), glm::value_ptr(proj));
            shader_set_matrix(&lightSourceShader, (char*)("view"), glm::value_ptr(view));
            shader_set_matrix(&lightSourceShader, (char*)("model"), glm::value_ptr(model));
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();

        lastFrame = currentFrame;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    shader_delete(&sp0);
    shader_delete(&lightSourceShader);
    glfwTerminate();
    return 0;
}

// TODO: Keeping this code in case I want to re-visit math stuff in OpenGL
    // TESTS
#if TESTS_ENABLED
    printf("\n***** TESTS *****\n");
    Vector2D testVector = {3, 4};
    float test_length = Length(testVector);
    printf("\nLength of vector { %.2f, %.2f } = %.2f\n", testVector.x, testVector.y, test_length);

    Vector2D testVectorB = { 4.f, 8.f };
    float test_dot = Dot_2D(testVector, testVectorB);
    printf("\n{ %.2f, %.2f } dot { %.2f, %.2f } = %.2f\n", 
           testVector.x, testVector.y, 
           testVectorB.x, testVectorB.y,
           test_dot);

    Vector2D testVectorC = testVector - testVectorB;
    printf("\nResult of vector { %.2f, %.2f } - { %.2f, %.2f } = { %.2f, %.2f }\n", 
           testVector.x, testVector.y, 
           testVectorB.x, testVectorB.y,
           testVectorC.x, testVectorC.y) ;

    testVectorC = testVector + testVectorB;
    printf("\nResult of vector { %.2f, %.2f } + { %.2f, %.2f } = { %.2f, %.2f }\n", 
           testVector.x, testVector.y, 
           testVectorB.x, testVectorB.y,
           testVectorC.x, testVectorC.y) ;

    Vector2D testVectorD = { 1.f, 0.f };
    Vector2D testVectorE = { 0.70710f, 0.70710f };
    printf("\ncos of vectors { %.2f, %.2f } & { %.2f, %.2f } = %.2f\n",
           testVectorD.x, testVectorD.y, 
           testVectorE.x, testVectorE.y,
           ncos(testVectorD, testVectorE)) ;

    printf("\n**** END TESTS *****\n");
    Vector2D pos = { -3, 4 };
    Vector2D forward = { 5, -2 };

    float points[] = {
        0.f, 0.f, 0.f,
        1.f, 6.f, 0.f,
        -6.f, 0.f, 0.f,
        -4.f, 7.f, 0.f,
        5.f, 5.f, 0.f,
        -3.f, 0.f, 0.f,
        -6.f, -3.5f, 0.f
    };

#define NUMBER_OF_VECTORS 7
    float posToPointVectors[NUMBER_OF_VECTORS * 2];
    for(int i = 0; i < NUMBER_OF_VECTORS; ++i)
    {
        Vector2D currentPoint = { points[i * 3], points[(i * 3) + 1] };
        Vector2D posToPointVec = currentPoint - pos;
        posToPointVectors[i * 3] = posToPointVec.x / 800.f;
        posToPointVectors[i * 3 + 1] = posToPointVec.y / 600.f;
        posToPointVectors[i * 3 + 2] = 0.f;
        points[i * 3] = points[i * 3] / 800.f;
        points[i * 3 + 1] = points[i * 3 + 1] / 600.f;
    }

    int is_in_front[NUMBER_OF_VECTORS];
    for(int i = 0; i < NUMBER_OF_VECTORS; ++i)
    {
        is_in_front[i] = Dot_2D(forward, { posToPointVectors[i * 3], posToPointVectors[i * 3 + 1] }) > 0.f;
    }

    uint PointsArray, PointsBuffer;
    glGenVertexArrays(1, &PointsArray);
    glGenBuffers(1, &PointsBuffer);

    glBindVertexArray(PointsArray);
    glBindBuffer(GL_ARRAY_BUFFER, PointsBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    uint VectorArray, VectorBuffer;
    glGenVertexArrays(1, &VectorArray);
    glGenBuffers(1, &VectorBuffer);

    glBindVertexArray(VectorArray);
    glBindBuffer(GL_ARRAY_BUFFER, VectorBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(posToPointVectors), posToPointVectors, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
