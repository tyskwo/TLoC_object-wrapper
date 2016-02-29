#version 330 core

// We need one out (used to be g_FragColor)
in vec3 v_color;
out vec3 o_color;

void main()
{
  // NOTE: Tex co-ords flipped in 't'
	o_color = v_color;
}
