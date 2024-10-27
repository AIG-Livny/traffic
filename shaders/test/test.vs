#version 330 core

precision mediump float;

layout (location = 0) in vec2 aPos;
layout (location = 1) in uint aPattern;
layout (location = 2) in vec4 aColor;

uniform mat4 VIEW;
uniform mat4 PROJECTION;
uniform float ZOOM;

void main(){
   mat4 MVP = PROJECTION * VIEW;
   gl_Position = MVP * vec4(aPos,0,1);
}