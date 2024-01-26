#version 450 core

in vec2 uv;
in vec3 normal;
out vec4 frag_color;

void main()
{
  vec2 color = step(0.1, uv) * step(0.1, 1.0 - uv);
  frag_color = vec4(normal, 1.0);
}
