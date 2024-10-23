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
   float zoom = floor(ZOOM)+1;

   vec2 pos    = aPos * zoom;
   vec4 pos4   = PROJECTION * VIEW *
   // To make infinite illusion just move grid by big cell size distance
   vec4(
      pos.x - floor(camPos.x/100.0/zoom)*100.0*zoom,
      pos.y - floor(camPos.y/100.0/zoom)*100.0*zoom,
      0.0, 1.0);

   vertPos  = pos4.xy / pos4.w;
   startPos = vertPos;
   color    = aColor;
   pattern  = aPattern;

   gl_Position = pos4;

}