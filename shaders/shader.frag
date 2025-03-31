#version 330 core

in vec2 texCoord;
in vec3 ourColor;
out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float texModX;
uniform float texModY;
uniform float mixValue;

void main()
{
  FragColor = mix(
      texture(texture1, vec2(texCoord.x + texModX, texCoord.y + texModY)),
      texture(texture2, vec2(texCoord.x + texModX, texCoord.y + texModY)),
      mixValue);
}
