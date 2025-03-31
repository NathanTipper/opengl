#ifndef SHADER_H
#define SHADER_H

enum ShaderType
{
	SHADERTYPE_VERTEX,
	SHADERTYPE_FRAGMENT,
	SHADERTYPE_COUNT
};

#define SHADER_SOURCE_FILENAME_SIZE 32
struct ShaderProgram
{
	unsigned int id;

	char shader_source[SHADERTYPE_COUNT][SHADER_SOURCE_FILENAME_SIZE];
	unsigned int shaders[SHADERTYPE_COUNT];
};

void shader_init(ShaderProgram* sp);
void shader_use(ShaderProgram* sp);
bool shader_link(ShaderProgram* sp);
void shader_set_int(ShaderProgram* sp, char* name, int value);
void shader_set_float(ShaderProgram* sp, char* name, float value);
void shader_set_source(ShaderProgram* sp, ShaderType st, char* filename);
void shader_set_matrix(ShaderProgram* sp, char* name, float* value);
void shader_set_vec3(ShaderProgram* sp, char* name, float v0, float v1, float v2);
bool shader_load(ShaderProgram* sp);
void shader_delete(ShaderProgram* sp);

#endif
