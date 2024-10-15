#version 330 core

precision mediump float;

layout (location = 0) in vec2 aVertPos;
layout (location = 1) in vec2 aPos;
layout (location = 2) in vec2 aDir;
layout (location = 3) in uint aPattern;
layout (location = 4) in vec4 aColor;
layout (location = 5) in float aWidth;

//flat out vec2 vert_pos;
//out vec2 pos;
//out vec2 dir;
out uint pattern;
out vec4 color;
out float width;

uniform mat4 VIEW;
uniform mat4 PROJECTION;
uniform float ZOOM;

#define camPos VIEW[3]

void main(){
   //vert_pos = aVertPos;
   //pos = aPos;
   //dir = aDir;
   pattern = aPattern;
   color = aColor;
   width = aWidth;
   gl_Position = vec4(aVertPos, 0.0, 1.0);
}