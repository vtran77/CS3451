#version 330 core

layout(std140) uniform camera {
    mat4 projection;
    mat4 view;
    mat4 pvm;
    mat4 ortho;
    vec4 position;
};

struct light {
    ivec4 att;
    vec4 pos;
    vec4 dir;
    vec4 amb;
    vec4 dif;
    vec4 spec;
    vec4 atten;
    vec4 r;
};
layout(std140) uniform lights {
    vec4 amb;
    ivec4 lt_att;
    light lt[4];
};

in vec3 vtx_normal;
in vec3 vtx_position;
in vec3 vtx_model_position;
in vec2 vtx_uv;

uniform sampler2D tex_color;
uniform vec3 planet_color = vec3(1.0);
uniform float atmosphere = 0.2;

out vec4 frag_color;

void main() {
    vec3 texture_color = texture(tex_color, vtx_uv).rgb * planet_color;
    
    vec3 N = normalize(vtx_normal);
    vec3 V = normalize(position.xyz - vtx_position);
    
    vec3 final_color = vec3(0.0);
    
    // Simple lighting
    for(int i = 0; i < lt_att[0]; i++) {
        vec3 L = normalize(lt[i].pos.xyz - vtx_position);
        vec3 H = normalize(V + L);
        
        vec3 ambient = lt[i].amb.rgb * 0.1;
        vec3 diffuse = lt[i].dif.rgb * max(dot(N, L), 0.0);
        vec3 specular = lt[i].spec.rgb * pow(max(dot(N, H), 0.0), 32.0) * 0.3;
        
        final_color += (ambient + diffuse + specular) * texture_color;
    }
    
    // Atmosphere glow at edges (rim lighting)
    float rim = pow(1.0 - max(dot(N, V), 0.0), 2.0);
    vec3 atmosphere_color = vec3(0.3, 0.5, 0.8); // Blue atmosphere
    final_color = mix(final_color, atmosphere_color, rim * atmosphere);
    
    frag_color = vec4(final_color, 1.0);
}