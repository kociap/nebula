#version 450 core

in vec2 uv;
in vec3 color;
out vec4 frag_color;

void main()
{
  frag_color = vec4(color, 1.0);
}
