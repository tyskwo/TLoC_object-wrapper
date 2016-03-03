#version 330 core

//Input vertex data, different for all executions of this shader.
//the position and normal of each vertice
	in      vec3 a_vertPos;
	in      vec3 a_vertNorm;

//the constants over the entire model: the model and vertice matrix
	uniform mat4 u_vp;
	uniform mat4 u_model;

//the light position and the color of the sphere
	        vec3 lightPosition;
	        vec3 color;

//color to pass to the fragment shader
	out     vec3 v_color;

void main()
{ 
//set the light position and the color of the sphere
	lightPosition = vec3(1.0, 1.0, 2.0);
	color		  = vec3(1.0, 0.0, 0.0);

//determine the position of the vertice
	gl_Position = u_vp * u_model * vec4(a_vertPos, 1);

//determine the normal of the light's direction
	vec3 lightDirection = lightPosition - a_vertPos;
	lightDirection = normalize(lightDirection);

//get the diffuse multiplier
	float diffuseMultiplier = dot(a_vertNorm, lightDirection);

//pass the calculated color to the fragment shader
	v_color = diffuseMultiplier * color;
}
