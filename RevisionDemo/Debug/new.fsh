#version 130
uniform vec2 iResolution;
uniform sampler2D iChannel0;


void main( ) {
    vec2 uv = gl_FragCoord.xy / iResolution;
    gl_FragColor = texture2D(iChannel0, uv)+vec4(1.,0.,0.,1.);
}