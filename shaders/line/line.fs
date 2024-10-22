#version 330 core

precision mediump float;

in vec4 fs_color;
flat in uint fs_pattern;
in float dist;

void main(){
   //uint bit = uint(round(dist)) & 15U;
   // if ((fs_pattern & (1U<<bit)) == 0U)
   //     discard; 

   gl_FragColor = fs_color;
}