#version 130
precision highp float;
uniform vec2 iResolution;
uniform float iGlobalTime;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
float rand(float co){
    return rand(vec2(co));
}
vec3 blur(sampler2D t, vec2 pos, float blur){
    vec3 c = vec3(0);
   
        for(float i=0.; i<20.; i++) {
            c.rgb += 
                texture2D(
                    t,
                    pos+
                    vec2(
                        sin(3.141591*2.*i/20.),
                        cos(3.141591*2.*i/20.)) * blur).rgb ;
        }
    	c.rgb /= 20.;
    return c;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec2 uv = gl_FragCoord.xy / iResolution.xy;     float aspect = iResolution.x / iResolution.y;

     vec2 p = 2.0 * gl_FragCoord.xy / iResolution.xy - 1.0;
    
	vec2 j = vec2(1.,0.)/ iResolution.xy;
	vec2 k = vec2(0.,1.)/ iResolution.xy;
    
    if(length(p.y)*2.>aspect ) {
        fragColor =  vec4(0.,0.,0.,0.);
    }else if(texture2D(iChannel0,uv).w<0.) {
        
        vec4 d = vec4(0.,0.,0.,-texture2D(iChannel0,uv).w)*4.;
        for(float i=1.;i<8.;i++) {
            d.rgb+=blur(iChannel0, uv, .001*i);
        }
        d/=7.;
        d.a = d.a*0.9 + texture2D(iChannel1,uv).a*0.2;
        d.rgb = d.rgb*0.4 + texture2D(iChannel0,uv).rgb*0.4 + texture2D(iChannel1,uv).rgb*0.2;
        fragColor = vec4(d);
    }else{
        vec4 d = texture2D(iChannel0,uv)*4. +
                texture2D(iChannel0,uv+j) +
                texture2D(iChannel0,uv+k) +
                texture2D(iChannel0,uv-j) +
                texture2D(iChannel0,uv-k) ;
        d /= 8.;
        d.a = d.a*0.9 + texture2D(iChannel1,uv).a*0.2;
        d.rgb = d.rgb*0.6 + texture2D(iChannel1,uv).rgb*0.4;
        if(length(p.y)*2.>aspect ) {
            fragColor =  vec4(0.,0.,0.,0.);
        }else if(texture2D(iChannel0,uv).w==0.)
            fragColor=vec4(0.,0.,0.,1.);
        else
            fragColor = vec4(d)+vec4(vec3(0.8,0.9,1.0) * 1.6 *(d.a*10.),0.0);
    }
}
    
void main( ) {
    vec4 color;
    vec2 coord= gl_FragCoord.xy / iResolution.xy;
    mainImage( color, coord );
    gl_FragColor = color;
}