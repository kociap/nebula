#version 450 core

in vec2 uv;
in vec3 color;
out vec4 frag_color;

void main()
{
  vec2 remapped = 2.0 * (uv - 0.5);
  float length_squared = length(remapped);
  frag_color.a = 1.0 - smoothstep(0.9, 1.0, length_squared);
  float color_factor = smoothstep(0.7, 0.8, length_squared);
  frag_color.rgb = mix(color, vec3(0.0), color_factor);
}
