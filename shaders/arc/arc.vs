#version 330 core

precision mediump float;

layout (location = 0) in vec2 aPos;
layout (location = 1) in float aRadius;
layout (location = 2) in float aStartDegree;
layout (location = 3) in float aEndDegree;
layout (location = 4) in vec4 aColor;
layout (location = 5) in uint aPattern;
layout (location = 6) in float aWidth;

out float radius;
out float start_degree;
out float end_degree;
out vec4 color;
out uint pattern;
out float width;

void main(){
   radius         = aRadius;
   start_degree   = aStartDegree;
   end_degree     = aEndDegree;
   color          = aColor;
   pattern        = aPattern;
   width          = aWidth;

   vec4 pos = vec4(aPos, 0.0, 1.0);
   gl_Position = pos;
}