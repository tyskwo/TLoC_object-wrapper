#version 330 core

// Input vertex data, different for all executions of this shader.
in vec3 a_vertPos;
in vec3 a_vertNorm;

uniform mat4 u_vp;
uniform mat4 u_model;

vec3 lightPosition;
vec3 color;

out vec3 v_color;

void main()
{ 
	lightPosition = vec3(1.0, 1.0, 1.0);
	color = vec3(1.0, 0.0, 0.0);

	gl_Position = u_vp * u_model * vec4(a_vertPos, 1);

	vec3 lightDirection = lightPosition - a_vertPos;
	lightDirection = normalize(lightDirection);

	float diffuseMultiplier = dot(a_vertNorm, lightDirection);

	v_color = diffuseMultiplier * color;
}
