#version 330 core

precision mediump float;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in uint aPattern;

uniform mat4 VIEW;
uniform mat4 PROJECTION;
uniform float ZOOM;

#define camPos VIEW[3] 

out vec4 color;
flat out uint pattern;
flat out vec2 startPos;
out vec2 vertPos;

void main(){
   vec4 pos = PROJECTION * VIEW * vec4(aPos, 0.0, 1.0);
   vertPos  = pos.xy / pos.w;
   startPos = vertPos;
   color    = aColor;
   pattern  = aPattern;

   gl_Position = pos;
}