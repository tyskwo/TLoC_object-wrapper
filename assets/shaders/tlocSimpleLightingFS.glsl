#version 330 core

//the color from the vertex shader
	in  vec3 v_color;

//the color to pass to the renderer
	out vec3 o_color;

void main()
{
//simply just pass the color
	o_color = v_color;
}
