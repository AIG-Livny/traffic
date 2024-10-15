#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices = 5) out;

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
    vec2 dir = normalize(gl_in[1].gl_Position - gl_in[0].gl_Position).xy;
    vec2 perp = vec2(-dir.y, dir.x);

    float d = 2/ZOOM;

    dist = d;
    gl_Position = MVP * (gl_in[1].gl_Position - vec4(perp * half_width,0,0));
    EmitVertex();
    gl_Position = MVP * (gl_in[1].gl_Position + vec4(perp * half_width,0,0));
    EmitVertex();

    dist = 0;
    gl_Position = MVP * (gl_in[0].gl_Position + vec4(perp * half_width,0,0));
    EmitVertex();
    gl_Position = MVP * (gl_in[0].gl_Position - vec4(perp * half_width,0,0));
    EmitVertex();
    gl_Position = MVP * (gl_in[1].gl_Position - vec4(perp * half_width,0,0));
    dist = d;
    EmitVertex();

    EndPrimitive();
}