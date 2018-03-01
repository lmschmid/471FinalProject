#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec2 texoff;
out vec3 vertex_pos;
out vec2 vertex_tex1;
out vec2 vertex_tex2;
uniform sampler2D n1;
uniform sampler2D h1;
uniform sampler2D n2;
uniform sampler2D h2;

uniform vec3 camoff;

void main()
{
	vec2 texcoords1=vertTex;
    vec2 texcoords2=vertTex;
    texcoords1 += texoff;
    texcoords2 += vec2(texoff.y, texoff.x);
	float t=1./100.;
	texcoords1 -= vec2(camoff.x,camoff.z)*t;
    texcoords2 -= vec2(camoff.x,camoff.z)*t;
    
	float height1 = texture(h1, texcoords1).r;
    float height2 = texture(h2, texcoords2).r;
    float tHeight = height1+height2;
    tHeight *= 2;

	vec4 tpos =  vec4(vertPos, 1.0);
	tpos.z -=camoff.z;
	tpos.x -=camoff.x;
	tpos =  M * tpos;
    tpos.y += tHeight;
	vertex_pos = tpos.xyz;
	gl_Position = P * V * tpos;
	vertex_tex1 = texcoords1;
    vertex_tex2 = texcoords2;
}
