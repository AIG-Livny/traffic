#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in vec2 pos[];
in vec2 dir[];
in uint pattern[];
in vec4 color[];
in float width[];

#define FLT_MAX 3.402823466e+38

uniform mat4 VIEW;
uniform mat4 PROJECTION;
uniform float ZOOM;

out vec4 fs_color;
flat out uint fs_pattern;
out float dist;

/*
float project(vec2 a, vec2 b){
    return cross(dot(a,b) / dot(b,b), b);
}
*/

void main(){
    mat4 MVP = PROJECTION * VIEW;
    fs_color = color[0];
    fs_pattern = pattern[0];

    float half_width = (width[0]/2.0) * (ZOOM);
    vec2 dir_norm = normalize(dir[0]);
    vec2 perp = vec2(-dir_norm.y, dir_norm.x);

    // Move along line to camera view to make initity illusion    
    vec4 repos = vec4(
        (dir_norm.x * VIEW[3].x) - (dir_norm.x * VIEW[3].y),
        (dir_norm.y * VIEW[3].x) - (dir_norm.y * VIEW[3].y) ,0,0);
    vec4 v4pos = vec4(pos[0],0,0);

    float d = 50/ZOOM;
    vec2 max_len = dir_norm * ZOOM * 1000; 
    
    dist = d;
    gl_Position = MVP * (vec4(max_len,0,1) - vec4(perp * half_width,0,0) + repos + v4pos);    
    EmitVertex();
    gl_Position = MVP * (vec4(max_len,0,1) + vec4(perp * half_width,0,0) + repos + v4pos);    
    EmitVertex();
    
    dist = 0;
    gl_Position = MVP * (vec4(-max_len,0,1) + vec4(perp * half_width,0,0) + repos + v4pos);    
    EmitVertex();
    gl_Position = MVP * (vec4(-max_len,0,1) - vec4(perp * half_width,0,0) + repos + v4pos);    
    EmitVertex();
    gl_Position = MVP * (vec4(max_len,0,1) - vec4(perp * half_width,0,0) + repos + v4pos);    
    dist = d;
    EmitVertex();

    
    EndPrimitive();
}