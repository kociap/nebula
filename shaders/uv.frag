#version 450 core

in vec2 uv;
out vec4 frag_color;

void main()
{
  frag_color = vec4(uv, 0.0, 1.0);
}
