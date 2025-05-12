#version 330 core

in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
void main()
{
  FragColor = vec4(0.f, 128.f, 0.f, 1.f);
}

