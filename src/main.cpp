#include <cstdio>
#include <glad/glad.h>
#include <glfw3.h>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
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

void processInput(GLFWwindow* window)
{
    float cameraSpeed = CAMERA_SPEED * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, 1);
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
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

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    int indices[] = {
        0,1,3,
        1,2,3
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

    uint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // LOAD TEXTURE
    unsigned int texture0;
    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("..\\data\\container.jpg", &width, &height, &nrChannels, 0);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        printf("Could not load file data\n");
    }

    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    data = stbi_load("..\\data\\awesomeface.png", &width, &height, &nrChannels, 0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        printf("Could not load file data\n");
    }

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
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        shader_set_int(&sp0, (char*)"texture1", 0);
        shader_set_int(&sp0, (char*)"texture2", 1);

        glm::mat4 proj = glm::mat4(1.f);

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
        proj = glm::perspective(glm::radians(fov), 800.f / 600.f , 0.1f, 100.f);
        float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader_set_matrix(&sp0, (char*)("projection"), glm::value_ptr(proj));
        shader_set_matrix(&sp0, (char*)("view"), glm::value_ptr(view));
        glBindVertexArray(VAO);
        for(uint i = 0; i < 10; ++i)
        {
            glm::mat4 model = glm::mat4(1.f);
            model = glm::translate(model, cubePositions[i]);
            if(i % 3 == 0)
            {
                float angle = (float)glfwGetTime() * glm::radians(45.f);
                model = glm::rotate(model, angle, glm::vec3(1.f, 0.3f, 0.5f));
            }
            shader_set_matrix(&sp0, (char*)"model", glm::value_ptr(model));
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
