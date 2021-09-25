#version 330

uniform sampler2D textureMap0;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 ic; 
in vec4 n;
in vec4 l;
in vec4 l2;
in vec4 v;
in vec2 iTexCoord0;

in float d;


void main(void) {

	//Znormalizowane interpolowane wektory
	vec4 ml = normalize(l);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	vec4 ml2 = normalize(l2);
	
	//Wektor odbity
	vec4 mr = reflect(-ml, mn);
	vec4 mr2 = reflect(-ml2, mn);
	
	//
	vec4 kd = texture(textureMap0, iTexCoord0); 
	vec4 ks = kd;

	
	float nl = clamp(dot(mn, ml), 0, 1);
	float nl2 = clamp(dot(mn, ml2), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1), 25);
	float rv2 = pow(clamp(dot(mr2, mv), 0, 1), 50);
	
	pixelColor = vec4(kd.rgb * nl, kd.a) + vec4(kd.rgb * nl2, kd.a); //Phong

}
