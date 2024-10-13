#version 330 core

precision mediump float;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in int aType;
layout (location = 3) in float aSize;

out int type;
out vec4 color;
out float size;

void main(){
   type = aType;
   color = aColor;
   size = aSize;

   gl_Position = vec4(aPos, 0.0, 1.0);
}