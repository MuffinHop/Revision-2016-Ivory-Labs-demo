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
float hash(float c){return fract(sin(dot(c,12.9898))*43758.5453);}

//Reinhard based tone mapping
vec3 tone(vec3 color, float gamma)
{
	float white = 2.;
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma * (1. + luma / (white*white)) / (1. + luma);
	color *= toneMappedLuma / luma;
	color = pow(color, vec3(1. / gamma));
	return color;
}


vec2 curve(vec2 uv)
{
	uv = (uv - 0.5) * 2.0;
	uv *= 1.1;	
	uv.x *= 1.0 + pow((abs(uv.y) / 5.0), 2.0);
	uv.y *= 1.0 + pow((abs(uv.x) / 4.0), 2.0);
	uv  = (uv / 2.0) + 0.5;
	uv =  uv *0.92 + 0.04;
	return uv;
}
vec3 boxblur(sampler2D t, vec2 pos, float blur){
    //if(blur < 0.3) return vec3(0.9);
    vec3 c = vec3(0);
   
    float it = 0.0;
    for(float i = 0.0; i < 40.0; i++){
        vec2 rval;
        rval.x = blur * rand(vec2(0.002345*iGlobalTime+pos.x, float(i)+pos.y)) - blur / 2.0;
        rval.y = blur * rand(vec2(pos.x + float(i), pos.y+0.007615*iGlobalTime)) - blur / 2.0;
        c += texture2D(t, pos + 0.25*rval*vec2(1.0/iResolution.x, 1.0/iResolution.y)).rgb;
        it += 1.0;
    }
    return c/it;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {  
     vec2 p = 2.0 * gl_FragCoord.xy / iResolution.xy - 1.0;
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    
    	float T=iGlobalTime*0.2;
	vec2 j = vec2(3.,0.)/ iResolution.xy;
	vec2 k = vec2(0.,3.)/ iResolution.xy;
    uv=curve(uv);
    vec4 C = vec4( 0., 0., 0., 1.);
    if(uv.x<0. || uv.x>1.) {
    	C = vec4(.0,.0,.0,1.0);
    } else {
        float dist = 
            texture2D(iChannel0,uv).w +
            texture2D(iChannel0,uv+j).w +
            texture2D(iChannel0,uv+k).w +
            texture2D(iChannel0,uv-j).w +
            texture2D(iChannel0,uv-k).w;
        dist /= 5.;
        dist = 0.04*dist*dist*dist*dist + 1./(1.+dist*64000.);
        for(float i=0.; i<80.; i++) {
            C.rgb += 
                texture2D(
                    iChannel0,
                    uv+
                    vec2(
                        sin(3.141591*2.*i/80.),
                        cos(3.141591*2.*i/80.)) *dist / (1.+T*.6)).rgb ;
        }
    	C.rgb /= 80.;
        C.rgb = 2.*C.rgb/3. + boxblur(iChannel0, uv, dist*4000.)/3.;
    }
    	C.rgb += vec3(floor(max( mod(uv.x*320.,32.)-30.,0.)))*length(p.y)*length(p.y)*length(p.y)/12.;
        //C.rgb = tone(C.rgb,2.1);
    	//C.rgb-=vec3(.1);
    	C.rgb *= 1.-pow(length(p.x),8.);
    	C.rgb *= 1.-pow(length(p.y),8.);
        //C.rgb=max(C.rgb,vec3(.08,.105,.09));
    	C.w=1.;
    	fragColor = C;
}
void main( ) {
    vec4 color;
    vec2 coord= gl_FragCoord.xy / iResolution.xy;
    mainImage( color, coord );
    gl_FragColor = color;
}