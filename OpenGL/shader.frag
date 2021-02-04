#version 120

varying vec3 f_color;
uniform float fade;

void main()
{        
    gl_FragColor = vec4(f_color.r + fade, f_color.g, f_color.b + fade, 0.5);
//    gl_FragColor = vec4(f_color.r, f_color.g, f_color.b + fade, 1.0);
}