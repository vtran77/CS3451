#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 v_color;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec4 uv;

out vec2 vtx_uv;

void main() {
    vtx_uv = uv.xy;
    gl_Position = vec4(pos.xyz, 1.0);
}