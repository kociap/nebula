#version 450 core

in vec2 uv;
out vec4 frag_color;

uniform vec3 camera_position;

void main()
{
    vec2 color = step(0.1, uv) * step(0.1, 1.0 - uv);
    frag_color = vec4(color.x, 0, 0, 0);
}
