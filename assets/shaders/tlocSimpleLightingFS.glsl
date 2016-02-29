#version 330 core

// We need one out (used to be g_FragColor)
out vec3 o_color;

void main()
{
  // NOTE: Tex co-ords flipped in 't'
	o_color = vec3(1.0, 0.0, 0.0);
}
