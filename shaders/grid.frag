#version 450 core

in vec2 position;
out vec4 frag_color;

uniform float zoom;

float log10(float value)
{
  return log(value) / log(10.0);
}

float grid_factor(vec2 position, float grid_size, vec2 grid_fwidth)
{
  vec2 grid = abs(mod(position, grid_size));
  grid /= grid_fwidth;
  float factor = min(grid.x, grid.y);
  return 1.0 - smoothstep(0.0, 2.0, factor);
}

void main()
{
  vec4 grid_color = vec4(1.0);

  float distance_linear = log10(max(zoom, 10.0));
  float distance_floor = floor(distance_linear);
  float distance_diff = distance_floor - (distance_linear - 1.0);
  vec2 grid_fwidth = fwidth(position);

  float scale_a = pow(10.0, distance_floor - 1.0);
  float scale_b = scale_a * 10.0;

  float grid_a = grid_factor(position, scale_a, grid_fwidth);
  float grid_b = grid_factor(position, scale_b, grid_fwidth);

  frag_color = grid_color;
  frag_color.a = grid_a * clamp(distance_diff * 2.0 - 1.0, 0.0, 1.0);
  frag_color = mix(frag_color, grid_color, grid_b);

  // frag_color.a = 1.0;
  // frag_color.rgb = vec3(grid_a);
}
