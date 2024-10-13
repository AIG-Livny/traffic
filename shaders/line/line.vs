#version 330 core

precision mediump float;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aDir;
layout (location = 2) in uint aPattern;
layout (location = 3) in vec4 aColor;
layout (location = 4) in float aWidth;

out vec2 pos;
out vec2 dir;
out uint pattern;
out vec4 color;
out float width;

void main(){
   pos = aPos;
   dir = aDir;
   pattern = aPattern;
   color = aColor;
   width = aWidth;
}