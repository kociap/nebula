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

uniform f32 zoom_level;
uniform vec3 camera_position;
uniform mat4 ortho_projection;

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

out vec2 uv;

void main()
{
  gl_Position = ortho_projection * vec4(get_position(gl_VertexID) - camera_position, 1.0);
  uv = get_uv(gl_VertexID);
}
