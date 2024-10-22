#version 330 core

precision mediump float;

in vec4 fs_color;

void main(){
   gl_FragColor = fs_color;
}