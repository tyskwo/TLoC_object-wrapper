#version 330 core

	in      vec3 a_vertPos;			//vertex position
	in      vec3 a_vertNorm;		//vertex normal

	uniform mat4 u_vp;				//the view projection matrix
	uniform mat4 u_model;			//the model
	uniform vec3 u_lightPosition;	//the light position

	out     vec3 v_vertPosition;	//the vertex's position
	out		vec3 v_vertNormal;		//the vertex's normal
	out		vec3 v_lightDirection;	//the light direction

void main()
{ 
//determine the position of the vertice
	gl_Position = u_vp * u_model * vec4(a_vertPos, 1);

	//TODO: need to transform the light's position based on the camera's transformations.

//determine the light direction
	v_lightDirection = u_lightPosition - a_vertPos;
	v_lightDirection = normalize(v_lightDirection);

//pass the position and normal to the fragment shader
	v_vertPosition  = a_vertPos;
	v_vertNormal    = a_vertNorm;
}
