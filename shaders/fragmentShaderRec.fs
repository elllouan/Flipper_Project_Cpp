#version 330 core

out vec4 fragColor;

in vec4 vertexColor;

uniform float myBlue;

void main()
{
	fragColor = vec4(vertexColor.rg, myBlue, 1.0f);
}