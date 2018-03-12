#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform vec3 campos;

uniform sampler2D skytex;

void main()
{
    vec4 tcol = texture(skytex, vertex_tex);
    color = tcol;
//    color.r = vertex_tex.y;
//    color.g = 0;
//    color.b = 0;
}
