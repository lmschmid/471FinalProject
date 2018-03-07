#version 330 core
out vec4 color;

in vec3 vertex_pos;
in vec2 vertex_tex1;
in vec2 vertex_tex2;
in vec2 wave_tex3;
in vec2 wave_tex4;

uniform sampler2D n1;
uniform sampler2D h1;
uniform sampler2D n2;
uniform sampler2D h2;
uniform sampler2D n3;
uniform sampler2D h3;
uniform sampler2D n4;
uniform sampler2D h4;

uniform vec3 camoff;
uniform vec3 campos;

void main()
{
    vec3 norm1 = texture(n1, vertex_tex1).rbg;
    vec3 norm2 = texture(n2, vertex_tex2).rbg;
    vec3 norm3 = texture(n3, wave_tex3).rbg;
    vec3 norm4 = texture(n4, wave_tex4).rbg;
    
    vec3 norm1R = (norm1+vec3(1,1,1))/2;
    vec3 norm2R = (norm2+vec3(1,1,1))/2;
    vec3 norm3R = (norm3+vec3(1,1,1))/2;
    vec3 norm4R = (norm4+vec3(1,1,1))/2;
    
    vec3 normal = normalize(norm1R + norm2R + norm3R + norm4R);
    normal = vec3(normal.x, normal.z, normal.y);

    vec2 texcoords1=vertex_tex1;
    vec2 texcoords2=vertex_tex2;

    vec3 heightcolor1 = texture(h1, vertex_tex1).rgb;
    vec3 heightcolor2 = texture(h2, vertex_tex2).rgb;
    vec3 heightcolor3 = texture(h3, wave_tex3).rgb;
    vec3 heightcolor4 = texture(h4, wave_tex4).rgb;

    heightcolor1.r = 0.1 + heightcolor1.r*0.9;
    heightcolor2.r = 0.1 + heightcolor2.r*0.9;
    heightcolor3.r = 0.1 + heightcolor3.r*0.9;
    heightcolor4.r = 0.1 + heightcolor4.r*0.9;
    float hFactor = heightcolor1.r+heightcolor2.r+heightcolor3.r+heightcolor4.r;
    vec3 oCol = vec3(.109,.419,1.);
    color.rgb = oCol*hFactor;

    float len = length(vertex_pos.xz+campos.xz);
    len-=41;
    len/=8.;
    len=clamp(len,0,1);
    color.a=1-len;

    //spec light
    vec3 lightp = vec3(0,10,-10);
    vec3 lightdir = normalize(lightp - vertex_pos);
    vec3 camdir = normalize(-campos-vertex_pos);
    vec3 h = normalize(lightdir + camdir);
    float spec = dot(normal, h);
    spec = clamp(spec, 0 , 1);
    spec = pow(spec, 10);
    color.rgb += vec3(1,1,1)*spec;
    //diffuse
    float diffuse = dot(normal,lightdir);
//    color*=diffuse/20;
}

