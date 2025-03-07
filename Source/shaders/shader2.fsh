#version 310 es
precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_tex0;


void main() {
    if (v_texCoord.y > 0.5) {
        FragColor = vec4(0.2, 0.2, 0.2, 1.0) * texture(u_tex0, v_texCoord);
    } else {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0) * texture(u_tex0, v_texCoord);
    }
}
