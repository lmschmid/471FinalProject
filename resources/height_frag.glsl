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
uniform sampler2D skytex;

uniform vec3 camoff;
uniform vec3 campos;

float map(float value, float inMin, float inMax, float outMin, float outMax) {
    return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

vec3 rotateX(vec3 v) {
    float yt = v.y;
    float angle = 45;
    v.y = v.y*cos(angle)-v.z*sin(angle);
    v.z = yt*sin(angle)+v.z*cos(angle);
    return v;
}

// Normalize reflection vector and everything going into calculating reflection
void main()
{
    vec3 norm1 = texture(n2, vertex_tex1).rbg;
    vec3 norm2 = texture(n2, vertex_tex2).rbg;
    vec3 norm3 = texture(n3, wave_tex3).rbg;
    vec3 norm4 = texture(n4, wave_tex4).rbg;
    
    vec3 norm1R = (norm1-vec3(.5,.5,.5))*2;
    vec3 norm2R = (norm2-vec3(.5,.5,.5))*2;
    vec3 norm3R = (norm3-vec3(.5,.5,.5))*2;
    vec3 norm4R = (norm4-vec3(.5,.5,.5))*2;

    vec3 normal = normalize(norm1R + norm2R + norm3R + norm4R);
    normal = normalize(normal+vec3(0,0,10));
    normal = vec3(normal.x, normal.z, normal.y);

    vec2 texcoords1=vertex_tex1;
    vec2 texcoords2=vertex_tex2;

    // Calculating reflection from skybox
    vec3 camdir = normalize(-campos-vertex_pos);
    vec3 reflection = camdir-2*dot(camdir,normal)*normal;
    reflection = normalize(reflection);
    float sky_coordY = (reflection.y)/2 +.5;
    float sky_coordX = reflection.z;
    
    if(reflection.x>0)
        sky_coordX=2-sky_coordX;
        
    sky_coordX+=1;
    sky_coordX/=-4;
    
    vec3 oCol = texture(skytex, vec2(1+sky_coordX, sky_coordY)).rgb;
    color.rgb = oCol;

    float len = length(vertex_pos.xz+campos.xz);
    len-=41;
    len/=8.;
    len=clamp(len,0,1);
    color.a=1-len;

    //spec light
    vec3 lightp = vec3(0,10,-100);
    vec3 lightdir = normalize(lightp - vertex_pos);
    vec3 h = normalize(lightdir + camdir);
    float spec = dot(normal, h);
    spec = clamp(spec, 0 , 1);
    spec = pow(spec, 10);
    color.rgb += vec3(1,1,1)*spec/3;
}
