#include <cmath>
#include <cstdio>
#include <glad/glad.h>
#include <glfw3.h>

typedef unsigned int uint;
void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, 1);
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

    // TODO: @ntipper temporary shaders!

    // Vertex Shader
    char* vertex_shader_source = (char*)"#version 330 core\n \
                         layout (location = 0) in vec3 aPos;\n \
                         layout (location = 1) in vec3 aColor;\n \
                         out vec3 ourColor; \n \
                         void main()\n \
                         {\n \
                              gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n \
                              ourColor = aColor;\n \
                        }\n";

    uint vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        // Log some info
        printf("Vertex Shader compilation failed: %s\n", infoLog);
    }
    else
    {
        printf("Vertex Shader compilation successful!\n");
    }

    // Fragment Shader
    char* fragment_shader0_source = (char*)"#version 330 core\n \
                         out vec4 FragColor;\n \
                         in vec3 ourColor; \n \
                         void main()\n \
                         {\n \
                              FragColor = vec4(ourColor, 1.f);\n \
                        }\n";

    uint fragment_shader0;
    fragment_shader0 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader0, 1, &fragment_shader0_source, NULL);
    glCompileShader(fragment_shader0);
    
    glGetShaderiv(fragment_shader0, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader0, 512, NULL, infoLog);
        // Log some info
        printf("Fragment Shader compilation failed: %s\n", infoLog);
    }
    else
    {
        printf("Fragment Shader compilation successful!\n");
    }
   
    uint shader_program0;
    shader_program0 = glCreateProgram();

    glAttachShader(shader_program0, vertex_shader);
    glAttachShader(shader_program0, fragment_shader0);
    glLinkProgram(shader_program0);
    glGetProgramiv(shader_program0, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program0, 512, NULL, infoLog);
        printf("Shader Program linking failed: %s\n", infoLog);
    }
    else
    {
        printf("Shader program linking succeeded!\n");
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader0);

    float triangle1_vertices[] = {
        -.5f, -0.5f, 0.0f, 1.0f, 0.f, 0.f,
        .5f, -0.5f, 0.0f, 0.f, 1.0f, 0.f,
        0.f, .5f, 0.0f, 0.f, 0.f, 1.f
    };

    uint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1_vertices), triangle1_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // "Unbind"
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program0);
        
        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) * 0.5f) + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shader_program0, "ourColor");
        glUniform4f(vertexColorLocation, 0.f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program0);
    glfwTerminate();
    return 0;
}
