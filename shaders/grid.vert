#version 450 core

struct Vertex {
  float position[3];
  float normal[3];
  float uv[2];
};

layout(binding = 1, std430) readonly buffer vertex_buffer
{
  Vertex vertices[];
};

vec3 get_position(int index)
{
  float position[3] = vertices[index].position;
  return vec3(position[0], position[1], position[2]);
}

vec2 get_uv(int index)
{
  float uv[2] = vertices[index].uv;
  return vec2(uv[0], uv[1]);
}

uniform float inv_aspect;
uniform mat4 v_mat;
uniform float zoom;
out vec2 position;

void main()
{
  vec4 vertex_position = vec4(get_position(gl_VertexID), 1.0);
  gl_Position = vertex_position;
  vec4 ar = vec4(1.0, inv_aspect, 1.0, 1.0);
  position = zoom * vec2(v_mat * ar * vertex_position);
}
