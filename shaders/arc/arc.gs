#version 330 core

#define VERTS_NUM 72
#define M_PI 3.1415926535897932384626433832795

layout (points) in;
layout (triangle_strip, max_vertices = 146) out;

in float radius[];
in float start_degree[];
in float end_degree[];
in vec4 color[];
in uint pattern[];
in float width[];

uniform mat4 VIEW;
uniform mat4 PROJECTION;
uniform float ZOOM;

out float angle;
out vec4 fs_color;
flat out uint fs_pattern;

void main(){
    mat4 MVP = PROJECTION * VIEW;
    fs_color = color[0];
    fs_pattern = pattern[0];

    float increment = (end_degree[0] - start_degree[0])/VERTS_NUM;
    for(float i=start_degree[0]; i<=end_degree[0]; i+=increment){
        vec4 base_pos = vec4(cos(i),sin(i), 0.0, 0.0);
        float half_width = (width[0]/2.0) * (ZOOM);
        angle = i + (2*M_PI);
    
        gl_Position = MVP * (base_pos * (radius[0] + half_width ) + gl_in[0].gl_Position);    
        EmitVertex();
        
        gl_Position = MVP * (base_pos * (radius[0] - half_width) + gl_in[0].gl_Position);
        EmitVertex();
    }

    EndPrimitive();
}