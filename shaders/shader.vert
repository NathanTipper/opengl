#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float hoffset;
out vec3 pos;
void main()
{
  pos = vec3(aPos.x, aPos.y, aPos.z);
  gl_Position = vec4(pos, 1.0);
}
