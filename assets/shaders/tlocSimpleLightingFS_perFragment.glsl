#version 330 core

//the position and normal of the vertex
	in		vec3 v_vertPosition;
	in      vec3 v_vertNormal;

//the light position, the color of the sphere, and the interpolated normal vector for each vertice
	        vec3 lightPosition;
	        vec3 color;
		    vec3 vertNorm_interpolated;

//the position of the light
	uniform vec3 u_lightPosition;

//the color to pass to the renderer
	out     vec3 o_color;

void main()
{
//set the color of the sphere
	color = vec3(1.0, 0.0, 0.0);

//determine the normal of the light's direction
	vec3 lightDirection = u_lightPosition - v_vertPosition;
	     lightDirection = normalize(lightDirection);

//normalize the interpolated normal
	vertNorm_interpolated = normalize(v_vertNormal);

//get the diffuse multiplier
	float diffuseMultiplier = dot(vertNorm_interpolated, lightDirection);

//pass the calculated color to the renderer
	o_color = diffuseMultiplier * color;
}
