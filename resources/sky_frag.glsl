#version 330 core
out vec4 color;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
uniform float cycle;

uniform sampler2D skytex;
uniform sampler2D nighttex;

void main()
{
    vec4 scol = texture(skytex, vertex_tex);
    vec4 ncol = texture(nighttex, vertex_tex);
    
    vec4 combo = scol*(1-cycle) + ncol*cycle;
    color = combo;
}
