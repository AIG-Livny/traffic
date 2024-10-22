#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in vec2 pos1[];
in vec2 pos2[];
in uint pattern[];
in vec4 color[];
in float width[];

uniform mat4 VIEW;
uniform mat4 PROJECTION;
uniform float ZOOM;

out vec4 fs_color;
flat out uint fs_pattern;
out float dist;

void main(){
    mat4 MVP = PROJECTION * VIEW;
    fs_color = color[0];
    fs_pattern = pattern[0];

    float half_width = (width[0]/2.0) * (ZOOM);
    vec2 dir = normalize(pos2[0] - pos1[0]);
    vec2 perp = vec2(-dir.y, dir.x);
    
    float d = 50/ZOOM;

    dist = d;
    gl_Position = MVP * (vec4(pos2[0],0,1) - vec4(perp * half_width,0,0));    
    EmitVertex();
    gl_Position = MVP * (vec4(pos2[0],0,1) + vec4(perp * half_width,0,0));    
    EmitVertex();
    
    dist = 0;
    gl_Position = MVP * (vec4(pos1[0],0,1) + vec4(perp * half_width,0,0));    
    EmitVertex();
    gl_Position = MVP * (vec4(pos1[0],0,1) - vec4(perp * half_width,0,0));    
    EmitVertex();
    gl_Position = MVP * (vec4(pos2[0],0,1) - vec4(perp * half_width,0,0));    
    dist = d;
    EmitVertex();

    
    EndPrimitive();
}