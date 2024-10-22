#version 330 core

#define M_PI 3.1415926535897932384626433832795
#define SIN_PI_3 0.866025403784

#define PLUS 0
#define CIRCLE 1
#define CROSS 2
#define DOT 3

layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

uniform mat4 PROJECTION;
uniform mat4 VIEW;
uniform float ZOOM;

in int type[];
in vec4 color[];
in float size[];

out vec4 fs_color;

void main(){
    mat4 MVP = PROJECTION * VIEW;
    fs_color = color[0];

    switch(type[0]){
        case PLUS:{
            float zoom = ZOOM * size[0];
            float width = 0.2;
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1, -width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1, width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1, width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1, -width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1, -width, 0.0, 0.0) * zoom);
            EmitVertex();
            
            EndPrimitive();

            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-width, 1, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(width, 1, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(width, -1, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-width, -1, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-width, 1, 0.0, 0.0) * zoom);
            EmitVertex();        
            }
            break;
        case CIRCLE:{
            float zoom = ZOOM * size[0];
            float zoomw = zoom * 0.5;
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0) * zoomw);
            EmitVertex();

            gl_Position = MVP * (gl_in[0].gl_Position + vec4(0.5, SIN_PI_3, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(0.5, SIN_PI_3, 0.0, 0.0) * zoomw);
            EmitVertex();

            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-0.5, SIN_PI_3, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-0.5, SIN_PI_3, 0.0, 0.0) * zoomw);
            EmitVertex();
            
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1, 0, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1, 0, 0.0, 0.0) * zoomw);
            EmitVertex();
            
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-0.5, -SIN_PI_3, 0, 0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-0.5, -SIN_PI_3, 0, 0) * zoomw);
            EmitVertex();

            gl_Position = MVP * (gl_in[0].gl_Position + vec4(0.5, -SIN_PI_3, 0, 0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(0.5, -SIN_PI_3, 0, 0) * zoomw);
            EmitVertex();

            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0) * zoomw);
            EmitVertex();
            }
            break;
        case CROSS:{    
            float zoom = ZOOM * size[0];
            float width = 0.1;
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1-width, 1-width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1+width, 1+width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1+width, -1+width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1-width, -1-width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1-width, 1-width, 0.0, 0.0) * zoom);
            EmitVertex();
            
            EndPrimitive();

            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1-width, 1+width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1+width, 1-width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1+width, -1-width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1-width, -1+width, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1-width, 1+width, 0.0, 0.0) * zoom);
            EmitVertex();        
            }
            break;
        case DOT:{
            float zoom = ZOOM * size[0];
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(0.5, SIN_PI_3, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-0.5, SIN_PI_3, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1, 0, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-1, 0, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(-0.5, -SIN_PI_3, 0, 0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(1, 0, 0.0, 0.0) * zoom);
            EmitVertex();
            gl_Position = MVP * (gl_in[0].gl_Position + vec4(0.5, -SIN_PI_3, 0, 0) * zoom);
            EmitVertex();
        }
        break;
    } 
    
    EndPrimitive();
}