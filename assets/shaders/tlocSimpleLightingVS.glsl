#version 330 core

// Input vertex data, different for all executions of this shader.
in vec3 a_vertPos;

uniform mat4 u_vp;
uniform mat4 u_model;

void main()
{ 
  gl_Position = u_vp * u_model * vec4(a_vertPos, 1);
}
