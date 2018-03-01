#version 330 core
out vec4 color;
in vec3 vertex_pos;
in vec2 vertex_tex1;
in vec2 vertex_tex2;

uniform sampler2D n1;
uniform sampler2D h1;
uniform sampler2D n2;
uniform sampler2D h2;

uniform vec3 camoff;
uniform vec3 campos;

void main()
{
    vec3 norm1 = texture(n1, vertex_tex1*10).rbg;
    vec3 norm2 = texture(n2, vertex_tex2*10).rbg;
    vec3 norm1R = (norm1+vec3(1,1,1))/2;
    vec3 norm2R = (norm2+vec3(1,1,1))/2;
    vec3 normal = normalize(norm1R+norm2R);
    normal = vec3(normal.x, normal.z, normal.y);

    vec2 texcoords1=vertex_tex1;
    vec2 texcoords2=vertex_tex2;
    float t=1./100.;
    texcoords1 -= vec2(camoff.x,camoff.z)*t;
    texcoords2 -= vec2(camoff.x,camoff.z)*t;

    vec3 heightcolor1 = texture(h1, vertex_tex1).rgb;
    vec3 heightcolor2 = texture(h2, vertex_tex2).rgb;

    heightcolor1.r = 0.1 + heightcolor1.r*0.9;
    heightcolor2.r = 0.1 + heightcolor2.r*0.9;
    float hFactor = heightcolor1.r+heightcolor2.r;
    color.rgb = texture(n1, texcoords1*50).rgb * hFactor;
    color.a=1;

    float len = length(vertex_pos.xz+campos.xz);
    len-=41;
    len/=8.;
    len=clamp(len,0,1);
    color.a=1-len;
    
    //spec light
    vec3 lightp = vec3(0,10,100);
    vec3 lightdir = lightp - vertex_pos;
    vec3 camdir = campos-vertex_pos;
    vec3 h = normalize(lightdir + camdir);
    float spec = dot(normal, h);
    spec = clamp(spec, 0 , 1);
    spec = pow(spec, 20);
    color.rgb += vec3(1,1,1)*spec;

}
