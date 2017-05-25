#version 130
uniform vec2 iResolution;


void main( ) {
    vec2 uv = gl_FragCoord.xy / iResolution;
    gl_FragColor = vec4(uv,0.4,1.0);
}
    