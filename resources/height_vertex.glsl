#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec2 texoff;
uniform vec2 texoff2;
out vec3 vertex_pos;
out vec2 vertex_tex1;
out vec2 vertex_tex2;
out vec2 wave_tex3;
out vec2 wave_tex4;
uniform sampler2D n1;
uniform sampler2D h1;
uniform sampler2D n2;
uniform sampler2D h2;
uniform sampler2D n3;
uniform sampler2D h3;
uniform sampler2D n4;
uniform sampler2D h4;

uniform vec3 camoff;

void main()
{
	vec2 texcoords1=vertTex;
    vec2 texcoords2=vertTex;
    vec2 texcoords3=vertTex;
    vec2 texcoords4=vertTex;
	float t=1./300.;
	texcoords1 -= vec2(camoff.x,camoff.z)*t;
    texcoords2 -= vec2(camoff.x,camoff.z)*t;
    texcoords3 -= vec2(camoff.x,camoff.z)*t;
    texcoords4 -= vec2(camoff.x,camoff.z)*t;
    
    texcoords1 += texoff;
    texcoords2 += vec2(texoff.y, texoff.x);
    texcoords3 += vec2(0.,texoff2.x-.0001);
    texcoords4 += texoff2;
    
	float height1 = texture(h2, texcoords1).r /2;
    float height2 = texture(h2, texcoords2).r/2;
    float height3 = texture(h3, texcoords3).r;
    float height4 = texture(h4, texcoords4).r;
    float tHeight = height1+height2+height3+height4;
    
	vec4 tpos =  vec4(vertPos, 1.0);
	tpos.z -=camoff.z;
	tpos.x -=camoff.x;
	tpos =  M * tpos;
    tpos.y += tHeight;
	vertex_pos = tpos.xyz;
	gl_Position = P * V * tpos;
	vertex_tex1 = texcoords1;
    vertex_tex2 = texcoords2;
    wave_tex3 = texcoords3;
    wave_tex4 = texcoords4;
}
