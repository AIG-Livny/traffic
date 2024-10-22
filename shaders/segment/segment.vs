#version 330 core

precision mediump float;

layout (location = 0) in vec2 aPos1;
layout (location = 1) in vec2 aPos2;
layout (location = 2) in uint aPattern;
layout (location = 3) in vec4 aColor;
layout (location = 4) in float aWidth;

out vec2 pos1;
out vec2 pos2;
out uint pattern;
out vec4 color;
out float width;

void main(){
   pos1 = aPos1;
   pos2 = aPos2;
   pattern = aPattern;
   color = aColor;
   width = aWidth;
}