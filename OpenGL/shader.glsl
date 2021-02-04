#version 120

attribute vec3 coord;
attribute vec3 v_color;
varying vec3 f_color;

uniform mat4 mat_transform;
uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(coord, 0.7);
    f_color = v_color;
}