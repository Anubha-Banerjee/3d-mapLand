# version 130

in vec2 mytexture;

varying vec4 color ;
varying vec3 mynormal ; 
varying vec4 myvertex ; 


uniform sampler2D colorMap2D;
uniform vec4 lightposn ; 
uniform vec4 lightcolor ; 
uniform vec4 ambient ; 
uniform vec4 diffuse ; 
uniform vec4 specular ; 
uniform float shininess ; 

vec4 ComputeLight (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in float myshininess) {

        float nDotL = dot(normal, direction)  ;         
        vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0)  ;  

        float nDotH = dot(normal, halfvec) ; 
        vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

        vec4 retval = lambert + phong ; 
        return retval ;            
}       

void main (void) 
{ 
    const vec3 eyepos = vec3(0,0,0) ; 
    vec4 _mypos = gl_ModelViewMatrix * myvertex ; 
    vec3 mypos = _mypos.xyz / _mypos.w ; // Dehomogenize current location 
    vec3 eyedirn = normalize(eyepos - mypos) ; 

    // Compute normal, needed for shading. 
    vec3 normal = normalize(gl_NormalMatrix * mynormal) ; 
    
	// Light point
    vec3 position = lightposn.xyz / lightposn.w ; 
    vec3 direction = normalize (position - mypos) ; // no attenuation 
    vec3 halfv = normalize (direction + eyedirn) ;  

    vec4 col = ComputeLight(direction, lightcolor, normal, halfv, diffuse, specular, shininess) ;
         
    gl_FragColor = ambient  * texture(colorMap2D, mytexture)+ col ; // vec4(mynormal, 1); 
      
}

