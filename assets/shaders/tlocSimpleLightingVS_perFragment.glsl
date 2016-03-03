#version 330 core

//Input vertex data, different for all executions of this shader.
//the position and normal of each vertice
	in      vec3 a_vertPos;
	in      vec3 a_vertNorm;

//the constants over the entire model: the model and vertice matrix
	uniform mat4 u_vp;
	uniform mat4 u_model;

//color to pass to the fragment shader
	out     vec3 v_vertPosition;
	out		vec3 v_vertNormal;

void main()
{ 
//determine the position of the vertice
	gl_Position = u_vp * u_model * vec4(a_vertPos, 1);

//pass the position and normal to the fragment shader
	v_vertPosition = a_vertPos;
	v_vertNormal   = a_vertNorm;
}
