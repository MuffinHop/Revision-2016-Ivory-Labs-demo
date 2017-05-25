#version 130
precision lowp float;
uniform vec2 iResolution;
uniform float iGlobalTime;
uniform float field_of_view;
uniform float moveground;
uniform vec3 cam_dist;
uniform vec3 cam_rot;
uniform vec3 dir_rot;

uniform vec3 lightSource;

#define MAX_STEPS 256.
#define epsilon 0.001
#define infinite 1e7
#define lightSize 6.
#define powLight 5.
#define lightColor vec3(1.0,0.92,0.83)
mat3 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
float rand(float co){
    return rand(vec2(co));
}
float smin( float a, float b, float k ) {
    float res = exp( -k*a ) + exp( -k*b );
    return -log( res )/k;
}
float sdSphere( vec3 p, float s ){
  return length(p)-s;
}
float udRoundBox( vec3 p, vec3 b, float r ) {
  return length(max(abs(p)-b,0.0))-r;
}
float udBox( vec3 p, vec3 b )
{
  return length(max(abs(p)-b,0.0));
}
float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) +
         length(max(d,0.0));
}
float sdHexPrism( vec3 p, vec2 h )
{
    vec3 q = abs(p);
    q=q.zxy;
    return max(q.z-h.y,max((q.x*0.866025+q.y*0.5),q.y)-h.x);
}

float length2( vec2 p ) {
	return sqrt( p.x*p.x + p.y*p.y );
}

float length6( vec2 p ) {
	p = p*p*p; p = p*p;
	return pow( p.x + p.y, 1.0/6.0 );
}

float length8( vec2 p ) {
	p = p*p; p = p*p; p = p*p;
	return pow( p.x + p.y, 1.0/8.0 );
}
float sdTorus88( vec3 p, vec2 t )
{
  vec2 q = vec2(length8(p.xz)-t.x,p.y);
  return length8(q)-t.y;
}
float sdTorus82( vec3 p, vec2 t )
{
  vec2 q = vec2(length2(p.xz)-t.x,p.y);
  return length8(q)-t.y;
}
float sdCappedCylinder( vec3 p, vec2 h )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}
float sdCapsule( vec3 p, vec3 a, vec3 b, float r )
{
    vec3 pa = p - a, ba = b - a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h ) - r;
}
mat3 rotY(in float a)
{
	return mat3( cos(a), 0.0, sin(a),
				 0.0,    1.0, 0.0,
				-sin(a), 0.0, cos(a)
			    );
}

mat2 rotate(float Angle)
{
    mat2 rotation = mat2(
        vec2( cos(Angle),  sin(Angle)),
        vec2(-sin(Angle),  cos(Angle))
    );
	return rotation;
}

vec2 hash( vec2 p )
{
	p = vec2( dot(p,vec2(127.1,311.7)),
			  dot(p,vec2(269.5,183.3)) );

	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2 i = floor( p + (p.x+p.y)*K1 );
	
    vec2 a = p - i + (i.x+i.y)*K2;
    vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0); //vec2 of = 0.5 + 0.5*vec2(sign(a.x-a.y), sign(a.y-a.x));
    vec2 b = a - o + K2;
	vec2 c = a - 1.0 + 2.0*K2;

    vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );

	vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));

    return dot( n, vec3(70.0) );
	
}
float getColorReflection( vec3 position, out vec3 color, out float reflectiveValue) {

    vec3 _position2 = position;
    vec3 _position = position+vec3(0.,1.0-length(position.xz*position.xz/32.)-length(position.x)/12.,0.);
    vec3 _position3 = _position;
    vec3 _position4 = _position;
    float dist = 1e8;
    float dist2 = 1e8;
    float time = iGlobalTime;
    float t2 = iGlobalTime*.1111;
    float b = noise( _position.xz/5.);
    float heightmap = (1.+0.1*length(position.xz))*noise( _position.xz/6.);
    float nh = noise( _position.xz*11.);
    heightmap += nh/44.;
    heightmap -= heightmap*nh/32.;
    heightmap += noise( _position.xz*2.)/8.;
    
    dist = min(
        dist,
        sdBox(
            _position - vec3(0.,-3.1,0.),
            vec3(256.,3.+heightmap,256.)));
    reflectiveValue = 0.2;
    
    float a=heightmap;
    	color = vec3(.45) - 2.*vec3(.4,.6,.8)*b*nh;
        color = min(max(color,vec3(0.)),vec3(1.));
    
    vec3 _positionX = position ;
	_positionX -=vec3( 0.0,2.0,0.0);
    _positionX = _positionX * rotationMatrix(vec3(0.1235,0.1,sin(iGlobalTime/23.)), iGlobalTime) ;
    float ass = 0.2+length(_positionX)*length(_positionX)/24.;
        dist2 = min( 
            dist2, 
            sdSphere( 
                _positionX, 0.67 ) );
        dist2 = min( 
            dist2, 
            sdBox( 
                _positionX, vec3(2.0,ass,ass) ) );
        dist2 = min( 
            dist2, 
            sdBox( 
                _positionX, vec3(ass,2.0,ass) ) );
        dist2 = min( 
            dist2, 
            sdBox( 
                _positionX, vec3(ass,ass,2.0) ) );
    if(dist2<dist) {
        dist = dist2;
        a=length(_positionX.y)*1.333+length(_positionX.x)*1.333+length(_positionX.z)*1.333;
        color = vec3(.05)+vec3(floor(mod(a*4.,2.))+.01)*vec3(1.,0.3,0.2)+vec3(floor(mod(a*4.,2.))+.01)*vec3(0.0,0.8,0.4);

        color += vec3(.05)+vec3(floor(mod(a*4.,2.))+.01)*vec3(1.,0.3,0.2)+vec3(floor(mod(a*4.,2.))+.01)*vec3(0.3,0.3,0.8);
        color*=2.;
        dist += length(color)/1024.;
        reflectiveValue = 0.4;
    }
    return dist;
}
float getDistance( in vec3 position) {
    vec3 _position = position;
    vec3 c;
    float r;
    return getColorReflection(_position, c, r);
}

vec3 getSurfaceNormal(vec3 position) {
    float e=epsilon;
    vec3 normalVector = vec3(
        					getDistance(position+vec3( e,  0., 0.)) - getDistance(position+vec3(  -e,   0.,  0.)) ,
        					getDistance(position+vec3( 0., e,  0.)) - getDistance(position+vec3(   0., -e,   0.)) ,
        					getDistance(position+vec3( 0., 0., e)) - getDistance(position+vec3(    0.,  0., -e)) );
    normalVector = normalize(normalVector);
    return normalVector;
}
float traceToLight(vec3 rayPosition, vec3 normalVector, vec3 lightSource, float raylightdist){

    vec3 ro = rayPosition;
    vec3 rd = normalize(lightSource - rayPosition);
    float t = 0.11;
    float k = distance(lightSource, rayPosition)/4.;
    float res = 1.0;
    for( int i=0; i<48; i++ )
    {
        float h = getDistance(ro + rd*t);
        h = max( h, 0.0 );
        res = min( res, k*h/t );
        t += clamp( h, 0.001, 0.9 );
        if( h<0.01) break;
    }
    return clamp(res,0.07,8.0);
}
float fastTraceToLight(vec3 rayPosition, vec3 normalVector, vec3 lightSource, float raylightdist){

    vec3 ro = rayPosition;
    vec3 rd = normalize(lightSource - rayPosition);
    float t = 0.2;
    float k = distance(lightSource, rayPosition)/4.;
    float res = 1.0;
    for( int i=0; i<11; i++ )
    {
        float h = getDistance(ro + rd*t);
        h = max( h, 0.0 );
        res = min( res, k*h/t );
        t += clamp( h, 0.001, 0.9 );
        if( h<0.01) break;
    }
    return clamp(res,0.07,9.0);
}
float getSurfaceColor( vec3 curPosition, vec3 normalVector, vec3 lightSource, vec3 lightDirection) {
    float intensity = lightSize * pow( 0.001 + max(0.0, dot( lightDirection, normalVector)),  powLight);
    intensity = lightSize * pow( intensity / distance( curPosition, lightSource),  powLight);
    intensity += pow (max( dot( normalize( lightSource - curPosition), normalVector), 0.0), powLight);
    return intensity;
}
vec4 passTrace(vec3 rayOrigin, vec3 rayDirection, out vec3 inside_colored) {
    vec3 endColorResult = vec3(0.0);
    vec3 prevPosition = rayOrigin;
	vec3 curPosition = rayOrigin;
    vec3 normalVector;
    vec4 result=vec4( 0., 0., 0., 0.);
    float finalLight = 1.0;
	float dist = 0.0;
    float storeColorStrength = 1.0;
    float globalIllumination=0.;
    float lengthGone = 0.0;
    float stepped = 0.0;
    #define LARGE_STEP 0.1
    /*raymarch through with bigger steps*/
        for(float i = 0.; i < 50.; i++) {
            stepped += LARGE_STEP;
            float stepable = getDistance( curPosition);
      
            curPosition += LARGE_STEP * rayDirection;
            lengthGone += LARGE_STEP;
            vec3 sLight = lightColor  * distance(curPosition, lightSource);
            inside_colored += LARGE_STEP * lightSize * sLight / (1.0 + pow( distance(lightSource, curPosition) ,  powLight) );
            if( getDistance( curPosition) > epsilon) {
                break;
            } 
            /*too far from begining	 point, call it an end*/
            if( distance( prevPosition.xy, curPosition.xy) > 8.) {
                break;
            }
        }
    inside_colored = inside_colored * lengthGone * 0.01;
    return vec4( curPosition, distance(prevPosition,curPosition));
}
vec3 AO(vec3 rayOrigin, vec3 rayDirection) {
    float AO = 0.0;
    for(float i=1.0; i<6.0; i++) {
        vec3 position =  rayOrigin + i * 0.05 * rayDirection;
        AO += getDistance( position);
    }
    return vec3(AO);
}
vec4 oneTrace(vec3 rayOrigin, vec3 rayDirection) {
    vec3 origRayDirection=rayDirection;
    vec3 endColorResult = vec3(0.0);
    vec3 prevPosition = rayOrigin;
	vec3 curPosition = rayOrigin + rayDirection/10.;
    vec3 normalVector;
    vec4 result=vec4( 0., 0., 0., 0.);
    vec3 finalLight = vec3(0.0);
	float dist = 0.0;
    float storeColorStrength = 1.0;
    int hit = 0;
    
        for(float i = 0.; i < 32.; i++) {
            float stepable = getDistance( curPosition);
            dist += stepable;
            curPosition = prevPosition + dist * rayDirection;

            if(length(curPosition*vec3(0.5,1.7,0.5))>8.0) break;
           /*too far from begining point, call it an end*/
            if( abs( stepable) < epsilon*4.) {
                vec3 color = vec3( 0.0);
                float R = 0.0;
                getColorReflection( curPosition, color, R);
                normalVector = getSurfaceNormal( curPosition);
                
                float raylightdist = distance( curPosition, lightSource);
                float light = traceToLight( curPosition, normalVector, lightSource, raylightdist);
                vec3 lightDirection = normalize( lightSource - curPosition);
                lightDirection = normalize(lightDirection);
                float directLight = dot(normalVector, lightDirection);
                float albedo = 0.9;
                finalLight += light * max( lightSize * directLight,0.01);
                float surfaceBrightness = length(finalLight);
                vec3 lightColorInside = color + finalLight * lightColor;
               
                vec3 surfaceFinalColor = color * lightColor * storeColorStrength * finalLight;
                
                float surfLight = getSurfaceColor(curPosition, normalVector, lightSource, lightDirection);
                float surfLightBackSide = 0.3333;
                
                float scatterDeathDepth = 0.3333;
                vec3 backSide = vec3(0.0);

                endColorResult += storeColorStrength * pow(max(dot(normalize(lightSource-curPosition),normalVector),0.0),powLight);
                endColorResult += storeColorStrength * (backSide + surfaceFinalColor) / (1.0+surfLight+surfLightBackSide);
                endColorResult += storeColorStrength * (backSide + surfaceFinalColor) * pow(lightColor,vec3(powLight))/ (1.0+surfLight+surfLightBackSide+scatterDeathDepth+pow(distance(curPosition,lightSource),2.) ) ;
                endColorResult += storeColorStrength * (backSide + surfaceFinalColor) * ( (surfLight+surfLightBackSide) ) * R * surfaceFinalColor ;
                
                storeColorStrength *= R / (1.0+scatterDeathDepth);
                hit = 1;
                dist = 0.0;
                break;
            }
    }
    
    
    vec3 lightDir = (lightSource-rayOrigin);
    lightDir = normalize(lightDir);
    float directLight = dot(rayDirection, lightDir);
    vec3 backdrop=min(max( pow(directLight,40.0) * vec3(1.8,1.1,.9) * 0.6, 0.01),1.);
    backdrop+=min(max( pow(directLight,1.5) * vec3(0.8,0.9,1.0) * 1.6, 0.01),1.);
    backdrop+=(min(max( pow(directLight,80.0) * 1.6, 0.01),1.)*1.1);
    
    vec3 farShoot=rayOrigin+origRayDirection*10.;
    backdrop+=vec3(0.,0.,.1);
  	if(hit==0 ) endColorResult += backdrop ;
    endColorResult = max(endColorResult,vec3(0.0));
    return vec4( endColorResult, 0.);
}
vec4 march(vec3 rayOrigin, vec3 rayDirection) {
    vec2 p = gl_FragCoord.xy / iResolution.xy;
    vec3 origRayDirection=rayDirection;
    vec3 endColorResult = vec3(0.0);
    vec3 prevPosition = rayOrigin;
	vec3 curPosition = rayOrigin;
    vec3 normalVector;
    vec4 result=vec4( 0., 0., 0., 0.);
    vec3 finalLight = vec3(0.0);
	float dist = 0.0;
    float storeColorStrength = 1.0;
    int hit = 0;
    float collisions = 0.;
    float distanceOfCollision = 1111110.;
    
    
    float distanceOfFirstHit = 0.;
    
    
        for(float i = 0.; i < MAX_STEPS; i++) {
            float R = 0.0;
                
            vec3 color = vec3( 0.0);
            float stepable = getColorReflection( curPosition, color, R);
            dist += stepable;
            curPosition = prevPosition + dist * rayDirection;
            if(length(curPosition*vec3(0.5,1.7,0.5))>8.0) break;
           	/*too far from begining point, call it an end*/
            if( abs( stepable) < epsilon + i*i/12000.) {
                normalVector = getSurfaceNormal( curPosition);
                
                float raylightdist = distance( curPosition, lightSource);
                float light = max(traceToLight( curPosition, normalVector, lightSource, raylightdist),0.3);
                vec3 lightDirection = normalize( lightSource - curPosition);
                lightDirection = normalize(lightDirection);
                float directLight = dot(normalVector, lightDirection);
                finalLight += light * max( lightSize * directLight,0.002);
                float surfaceBrightness = length(finalLight);
                vec3 lightColorInside = color + finalLight * lightColor;
               
                vec3 surfaceFinalColor = color * lightColor * storeColorStrength * finalLight;
                
                float surfLight = getSurfaceColor(curPosition, normalVector, lightSource, lightDirection);
				float surfLightBackSide =0.;  
                if(R<=0.3) {
                    endColorResult += 0.1 * finalLight * storeColorStrength * surfaceFinalColor * pow(max(dot(normalize(lightSource-curPosition),normalVector),0.0),powLight);
                    endColorResult += finalLight * storeColorStrength * surfaceFinalColor / (1.0+surfLight);
                    endColorResult += finalLight * storeColorStrength * surfaceFinalColor * pow(lightColor,vec3(powLight))/ (1.0+surfLight )  ;
                    endColorResult += finalLight * storeColorStrength * surfaceFinalColor * surfLight * R * surfaceFinalColor ;

                } else {
                    vec4 pos_ilum = passTrace( curPosition + epsilon * rayDirection, normalize(rayDirection*rotationMatrix(normalVector,0.1)), lightColorInside);

                    float surfLight = getSurfaceColor(curPosition, normalVector, lightSource, lightDirection);
                    surfLightBackSide = getSurfaceColor(-pos_ilum.xyz, -normalVector, lightSource, lightDirection);

                    float scatterDeathDepth = pow(pos_ilum.w,4.0)*122.0;
                    vec3 backSide = ( lightColorInside + color * lightColor * storeColorStrength *  oneTrace(pos_ilum.xyz + 2. * epsilon * rayDirection, rayDirection).rgb ) / (1.0 + scatterDeathDepth);

                    endColorResult += lightColorInside*12.;
                    endColorResult += (lightColor + color)/2. * storeColorStrength * pow(max(dot(normalize(lightSource-curPosition),normalVector),0.0),powLight);
                    endColorResult += storeColorStrength * (backSide + surfaceFinalColor) / (1.0+surfLight+surfLightBackSide);
                    endColorResult += storeColorStrength * (backSide + surfaceFinalColor) * pow(lightColor,vec3(powLight))/ (1.0+surfLight+surfLightBackSide+scatterDeathDepth+pow(distance(curPosition,lightSource),2.) ) ;
                    endColorResult += storeColorStrength * (backSide + surfaceFinalColor) * ( (surfLight+surfLightBackSide) ) * R * surfaceFinalColor / (1.0+pow(distance( curPosition, pos_ilum.xyz), 6.2 ));
       				
                    
                }
                vec3 AOresult = log(AO(curPosition, normalVector)) /5. ;
                endColorResult +=  AOresult /(1.0+(surfLight+surfLightBackSide));

                if(collisions==0.) {
                	distanceOfFirstHit = dist/32.;
                }
                storeColorStrength *= R;
                hit = 1;
                dist = 0.0;

                prevPosition = curPosition + normalVector*0.1;
                curPosition = prevPosition;
                rayDirection = normalize(reflect(rayDirection, -normalVector));
                collisions++;
                break;
            }
    }
    
    
    if(hit==0) distanceOfFirstHit = 0.1;
    
    vec3 lightDir = (lightSource-rayOrigin);
    lightDir = normalize(lightDir);
    float directLight = dot(rayDirection, lightDir);
    vec3 backdrop=min(max( pow(directLight,40.0) * vec3(1.8,1.1,.9) * 0.6, 0.01),1.);
    backdrop+=min(max( pow(directLight,1.5) * vec3(0.8,0.9,1.0) * 1.6, 0.01),1.);
    backdrop+=(min(max( pow(directLight,80.0) * 1.6, 0.01),1.)*1.1);
    
    vec3 farShoot=rayOrigin+origRayDirection*10.;
    backdrop+=vec3(0.,0.,.1);
    backdrop*=min(max(farShoot.y-0.2,0.),1.);
  	if(hit==0 ) endColorResult += backdrop ;
    endColorResult = max(endColorResult,vec3(0.0));
    if(collisions>0.) distanceOfFirstHit = -distanceOfFirstHit;
    return vec4( endColorResult, distanceOfFirstHit);
}
float resolveRaySphereIntersection(vec3 b, vec3 c, vec3 dir){
	vec3 OC=b-c;
	float P=dot(OC,dir);
	if(P<0.) return 1111.0;
	float d=sqrt(pow(length(OC),2.0)-pow(P,2.0));
	return d;
}
float trace(vec3 rayOrigin, vec3 rayDirection) {
    return resolveRaySphereIntersection(vec3(0.0,3.0,2.0), rayOrigin, rayDirection)*3.0;
}
float hash(float c){return fract(sin(dot(c,12.9898))*43758.5453);}


vec4 render( vec2 uv ) {  
     float aspect = iResolution.x / iResolution.y;
    	float T=iGlobalTime*0.1;
    	float vieteri = cos(T)/(1.+T*T*T);
        vec3 cameraPosition = cam_dist.xyz;
        vec3 direction = normalize( vec3(.5 * uv * vec2( aspect, 1.0), field_of_view) );



        direction *= rotationMatrix(vec3(1.0,0.0,0.0), dir_rot.x);
        direction *= rotationMatrix(vec3(0.0,1.0,0.0), dir_rot.y);
        direction *= rotationMatrix(vec3(0.0,0.0,1.0), dir_rot.z);
        cameraPosition *= rotationMatrix(vec3(1.0,0.0,0.0), -cam_rot.x);
        cameraPosition *= rotationMatrix(vec3(0.0,1.0,0.0), -cam_rot.y);
        cameraPosition *= rotationMatrix(vec3(0.0,0.0,1.0), -cam_rot.z);

        direction.z *= -1.0;
    
        vec4 color = march( cameraPosition, direction);

        return color;
        
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {  
     vec2 p = gl_FragCoord.xy / iResolution.xy;
     vec2 uv = 2.0 * gl_FragCoord.xy / iResolution.xy - 1.0;
     float aspect = iResolution.x / iResolution.y;
    if(length(uv.y)*2.>aspect ) {
		fragColor =  vec4(0.,0.,0.,1.);
    } else {
        vec4 C=render( uv );
		fragColor =  vec4(C.rgb,C.w/4.);
    }
}
void main( ) {
    vec4 color;
    vec2 coord= gl_FragCoord.xy / iResolution.xy;
    mainImage( color, coord );
    gl_FragColor = color;
}