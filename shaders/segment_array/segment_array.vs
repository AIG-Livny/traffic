#version 330 core

precision mediump float;

layout (location = 0) in vec2 aVertPos;
layout (location = 1) in uint aPattern;
layout (location = 2) in vec4 aColor;
layout (location = 3) in float aWidth;
layout (location = 4) in vec4 aTransform1;
layout (location = 5) in vec4 aTransform2;
layout (location = 6) in vec4 aTransform3;
layout (location = 7) in vec4 aTransform4;

out uint pattern;
out vec4 color;
out float width;

uniform mat4 VIEW;
uniform mat4 PROJECTION;
//uniform float ZOOM;

#define camPos VIEW[3]

void main(){
   pattern = aPattern;
   color = aColor;
   width = aWidth;
   mat4 t = mat4(aTransform1, aTransform2, aTransform3, aTransform4);
   gl_Position = t * vec4(vec3(aVertPos,0), 1.0);
   //gl_Position = vec4(vec3(aVertPos,0), 1.0);
}