#include <cstdio>
#include <glad/glad.h>
#include <glfw3.h>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
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
#define TEXTURE_COORD_MOVEMENT_SPEED 0.0001f
#define MIX_VALUE_DELTA_VALUE 0.0001f

global_variable float input_modifier_x;
global_variable float input_modifier_y;
global_variable float mix_value;

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, 1);
    }
    else if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        input_modifier_y += TEXTURE_COORD_MOVEMENT_SPEED;
    }
    else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        input_modifier_y -= TEXTURE_COORD_MOVEMENT_SPEED;
    }
    else if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        input_modifier_x += TEXTURE_COORD_MOVEMENT_SPEED;
    }
    else if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        input_modifier_x -= TEXTURE_COORD_MOVEMENT_SPEED;
    }
    else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        mix_value -= MIX_VALUE_DELTA_VALUE;
    }
    else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        mix_value += MIX_VALUE_DELTA_VALUE;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void)
{
    input_modifier_x = 0;
    input_modifier_y = 0;
    mix_value = 0;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    ShaderProgram sp0;
    shader_init(&sp0);
    shader_set_source(&sp0, SHADERTYPE_VERTEX, (char*)"..\\shaders\\shader.vert");
    shader_set_source(&sp0, SHADERTYPE_FRAGMENT, (char*)"..\\shaders\\shader.frag");
    shader_load(&sp0);
    shader_link(&sp0);

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
    while(!glfwWindowShouldClose(window))
    {
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
        glm::mat4 view = glm::mat4(1.f);

        proj = glm::perspective(glm::radians(30.f), 800.f / 600.f , 0.1f, 100.f);
        view = glm::translate(view, glm::vec3(.5f, 0.5f, -8.0f));
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

        shader_set_float(&sp0, (char*)"texModX", input_modifier_x);
        shader_set_float(&sp0, (char*)"texModY", input_modifier_y);
        shader_set_float(&sp0, (char*)"mixValue", mix_value);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    shader_delete(&sp0);
    glfwTerminate();
    return 0;
}
