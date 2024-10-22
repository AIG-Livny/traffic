#version 330 core

precision mediump float;

flat in vec2 startPos;
in vec2 vertPos;
in vec4 color;
flat in uint pattern;

void main(){
   float dist = length(vertPos-startPos);

   uint bit = uint(round(dist / 0.01)) & 15U;
    if ((pattern & (1U<<bit)) == 0U)
        discard; 

   gl_FragColor = color;
}