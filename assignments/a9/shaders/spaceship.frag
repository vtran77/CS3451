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
in vec3 vtx_tangent;

uniform sampler2D tex_color;
uniform sampler2D tex_normal;

out vec4 frag_color;

void main() {
    // Simple texture sampling
    vec3 texture_color = texture(tex_color, vtx_uv).rgb;
    
    // Simple lighting
    vec3 N = normalize(vtx_normal);
    vec3 V = normalize(position.xyz - vtx_position);
    
    vec3 final_color = vec3(0.0);
    
    // Just use first light
    if(lt_att[0] > 0) {
        vec3 L = normalize(lt[0].pos.xyz - vtx_position);
        vec3 H = normalize(V + L);
        
        vec3 ambient = lt[0].amb.rgb * 0.1;
        vec3 diffuse = lt[0].dif.rgb * max(dot(N, L), 0.0);
        vec3 specular = lt[0].spec.rgb * pow(max(dot(N, H), 0.0), 64.0);
        
        final_color = (ambient + diffuse + specular) * texture_color;
    } else {
        final_color = texture_color;
    }
    
    frag_color = vec4(final_color, 1.0);
}