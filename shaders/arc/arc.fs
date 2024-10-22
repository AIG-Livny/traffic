#version 330 core

precision mediump float;

uniform vec4 COLOR;
uniform uint PATTERN = uint(0xFFFF);
uniform float ZOOM;

in float angle;
in vec4 fs_color;
flat in uint fs_pattern;

#define M_PI 3.1415926535897932384626433832795

void main(){
   float u_factor=0.05;
   uint bit = uint(round(angle/ZOOM / u_factor)) & 15U;
    if ((fs_pattern & (1U<<bit)) == 0U)
        discard; 
   
   gl_FragColor = fs_color;
}