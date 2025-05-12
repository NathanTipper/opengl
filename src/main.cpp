#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>
#include <glad/glad.h>
#include <glfw3.h>
#include "assimp/material.h"
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
// TODO:@ntipper ^ Write my own image importer as a fun project!


#include "utils.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "nmath.h"

// Model loading using assimp
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <vector>

typedef unsigned int uint;
#define global_variable static
#define CAMERA_SPEED 5.f
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

#define LOG(a) printf("\n%s\n", a);
struct Vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tc;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Texture 
{
    uint id;
    char* type;
    aiString path;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::vector<Texture> textures;

    uint VAO;
    uint VBO;
    uint EBO;
};

struct Hex
{
    glm::vec3 position;
    glm::vec3 vertices[7];
    uint indices[18];
    glm::vec3 color;

    uint VAO;
    uint VBO;
    uint EBO;
};

#define MODEL_DIRECTORY_BUFFER_SIZE 128
struct Model
{
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;

    char directory[MODEL_DIRECTORY_BUFFER_SIZE];
};

#define PATH_BUFFER_SIZE 256
uint TextureFromFile(char* filename, char* directory, bool gamma)
{
    uint Result = 0;

    char path[PATH_BUFFER_SIZE];
    path[0] = '\0';
    strcat_s(path, PATH_BUFFER_SIZE, directory);
    strcat_s(path, PATH_BUFFER_SIZE, filename);
    glGenTextures(1, &Result);
    glBindTexture(GL_TEXTURE_2D, Result);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

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

void mesh_setup(Mesh* mesh)
{
    if(!mesh)
    {
        return;
    }

    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(uint), &mesh->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tc));
}

#define MESH_DRAW_DIGIT_MAX 256
void mesh_draw(Mesh* mesh, ShaderProgram* shader)
{
    uint diffuseNr = 1;
    uint specularNr = 1;

    char uniformName[512];
    for(unsigned int i = 0; i < mesh->textures.size(); ++i)
    {
        //strcat_s(uniformName, "material.");
        glActiveTexture(GL_TEXTURE0 + i);
        char number[MESH_DRAW_DIGIT_MAX];
        strcat_s(uniformName, mesh->textures[i].type);

        if(stringEqual(mesh->textures[i].type, (char*)"texture_diffuse"))
        {
            intToString(diffuseNr++, number, MESH_DRAW_DIGIT_MAX);
        }
        else if(stringEqual(mesh->textures[i].type, (char*)"texture_specular"))
        {
            intToString(specularNr, number, MESH_DRAW_DIGIT_MAX);
        }

        strcat_s(uniformName, number);
        shader_set_int(shader, uniformName, i);
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);

        uniformName[0] = '\0';
    }


    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, static_cast<uint>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

static std::vector<Texture>
loadMaterialTextures(Model* model, aiMaterial* mat, aiTextureType type, char* typeName)
{
    std::vector<Texture> textures;
    for(uint i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(uint j = 0; j < model->textures_loaded.size(); ++j)
        {
            if(std::strcmp(model->textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
            {
                textures.push_back(model->textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if(!skip)
        {
            Texture texture;
            texture.id = TextureFromFile(const_cast<char*>(str.C_Str()), model->directory, false);
            texture.type = typeName;
            texture.path = str;
            textures.push_back(texture);
            model->textures_loaded.push_back(texture);
        }
    }
    return textures;
}

static Mesh 
mesh_process(Model* model, aiMesh* mesh, const aiScene* scene)
{
    Mesh Result;

    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::vector<Texture> textures;

    for(uint i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if(mesh->mTextureCoords[0])
        {
            vertex.tc.x = mesh->mTextureCoords[0][i].x;
            vertex.tc.y = mesh->mTextureCoords[0][i].y;

            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;

            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        }
        else
        {
            vertex.tc = glm::vec2(0.f, 0.f);
        }
        vertices.push_back(vertex);
    }

    for(uint i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for(uint j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(model, mat, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(model, mat, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = loadMaterialTextures(model, mat, aiTextureType_NORMALS, "texture_normal");
        normalMaps.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> heightMaps = loadMaterialTextures(model, mat, aiTextureType_HEIGHT, "texture_height");
        normalMaps.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }

    Result.vertices = vertices;
    Result.indices = indices;
    Result.textures = textures;

    mesh_setup(&Result);

    return Result;
}

static void
model_process_node(Model* model, aiNode* node, const aiScene* scene)
{
    for(uint i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes.push_back(mesh_process(model, mesh, scene));
    }

    for(uint i = 0; i < node->mNumChildren; ++i)
    {
        model_process_node(model, node->mChildren[i], scene);
    }
}

static void
model_load(Model* model, char* path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("\nASSIMP COULD NOT LOAD MODEL. Error: %s\n", import.GetErrorString());
        return;
    }

    substring(model->directory, MODEL_DIRECTORY_BUFFER_SIZE, path, 0, findLastInString(path, '\\'));
    model_process_node(model, scene->mRootNode, scene);
}

static void
model_draw(Model* model, ShaderProgram* shader)
{
    for(uint i = 0; i < model->meshes.size(); ++i)
    {
        mesh_draw(&model->meshes[i], shader);
    }
}

static Hex 
MakeHex(glm::vec3 center, float sideLength, glm::vec3 color)
{
    Hex Result;
    float orientation = 30.0f;
    glm::vec3 currentVertex = glm::vec3(0.f);
    glm::vec3 startingPosition = { center.x + sideLength, center.y, center.z };
    glm::mat3 rotationMatrix = glm::mat3(1.0f);

    Result.vertices[0] = center;
    for(int i = 1; i < 7; ++i)
    {
        float vertexRotation = orientation + (60.0f * (i - 1));
        rotationMatrix[0][0] = cos(glm::radians(vertexRotation));
        rotationMatrix[0][2] = -sin(glm::radians(vertexRotation));
        rotationMatrix[2][0] = sin(glm::radians(vertexRotation));
        rotationMatrix[2][2] = cos(glm::radians(vertexRotation));

        glm::vec3 vertexPosition = rotationMatrix * startingPosition;
        Result.vertices[i] = vertexPosition;
    }

    uint indicesIndex = 0;
    for(uint i = 1; i < 7; ++i)
    {
        uint next = i+1 >= 7 ? 1 : i + 1;
        Result.indices[indicesIndex++] = 0;
        Result.indices[indicesIndex++] = i;
        Result.indices[indicesIndex++] = next;
        printf("\nFace index: %u\nNext index: %u\nIndices index: %u\n", i, next, indicesIndex);
    }

    Result.color = color;
    Result.position = center;

    glGenVertexArrays(1, &Result.VAO);
    glGenBuffers(1, &Result.VBO);
    glGenBuffers(1, &Result.EBO);

    glBindVertexArray(Result.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Result.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Result.vertices), &Result.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Result.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Result.indices), &Result.indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return Result;
}

static bool
CreateHexMap(uint hex_size, uint rows, uint cols, Hex* out_hex_map)
{
    if(!out_hex_map)
    {
        return false;
    }

    for(uint i = 0; i < rows; ++i)
    {
        for(uint j = 0; j < cols; ++j)
        {
        }
    }
    return true;
}

static void
DrawHex(Hex* hex)
{
    glBindVertexArray(hex->VAO);
    glDrawElements(GL_TRIANGLES, sizeof(hex->vertices), GL_UNSIGNED_INT, 0);
}

void processInput(GLFWwindow* window)
{
    float cameraSpeed = CAMERA_SPEED * deltaTime;
    glm::vec3 projCameraFrontToXZ = cameraFront;
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
    cameraPos = glm::vec3(0.f, 2.f, 3.0f);
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

    ShaderProgram hex_shader;
    shader_init(&hex_shader);
    shader_set_source(&hex_shader, SHADERTYPE_VERTEX, (char*)"..\\shaders\\shader.vert");
    shader_set_source(&hex_shader, SHADERTYPE_FRAGMENT, (char*)"..\\shaders\\shader.frag");
    shader_load(&hex_shader);
    shader_link(&hex_shader);

    // "Unbind"
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
    float currentFrame = 0.f;

    // TODO: MAKE FUNCTION CALLED MAKE HEX MAP THAT TAKES COLUMN AND ROW PARAMETERS
    // Model backpack;
    // model_load(&backpack, (char*)"..\\data\\backpack\\backpack.obj");
    while(!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 proj = glm::mat4(1.f);

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
        proj = glm::perspective(glm::radians(fov), 800.f / 600.f , 0.1f, 100.f);
        glm::mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        shader_use(&hex_shader);
        shader_set_matrix(&hex_shader, (char*)("projection"), glm::value_ptr(proj));
        shader_set_matrix(&hex_shader, (char*)("view"), glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));
        // MODEL STUFF
        shader_set_matrix(&hex_shader, (char*)"model", glm::value_ptr(model));
        DrawHex(&hex);
        glfwSwapBuffers(window);
        glfwPollEvents();

        lastFrame = currentFrame;
    }

    shader_delete(&hex_shader);
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
