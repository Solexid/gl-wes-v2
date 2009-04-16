#define NEVER                          0
#define LESS                           1
#define EQUAL                          2
#define LEQUAL                         3
#define GREATER                        4
#define NOTEQUAL                       5
#define GEQUAL                         6
#define ALWAYS                         7

uniform lowp vec4		uFogColor;
uniform sampler2D		uTexture0; 

//Enables:
uniform	bool			uEnableAlphaTest;
uniform	bool			uEnableTextureGenS;
uniform	bool			uEnableTextureGenT;
uniform	bool			uEnableTextureGenR;
uniform	bool			uEnableTextureGenQ;
uniform	bool			uEnableClipPlane[6];
uniform	bool			uEnableLighting;
uniform	bool			uEnableFog;

uniform	int				uAlphaFunc;
uniform	highp float		uAlphaRef;

//Varyings:
varying lowp vec4 		vColor0;
varying lowp vec4 		vColor1;
varying mediump vec4 	vTexCoord0;
varying mediump vec4 	vTexCoord1;
varying mediump float 	vFogFactor;
varying mediump float 	vClipFactor;

void main(){
	gl_FragColor = texture2D(uTexture0, vTexCoord0.xy);
	gl_FragColor *= vColor0; 
	
	if (uEnableFog){
		gl_FragColor = gl_FragColor * vFogFactor + (1.0 - vFogFactor) * uFogColor;
	}
	if (uEnableClipPlane[0]){	
		gl_FragColor.w *= vClipFactor;
	}
	
	if (uEnableAlphaTest){
		if (uAlphaFunc == NEVER){				discard;
		} else if (uAlphaFunc == LESS){
			if (gl_FragColor.w >= uAlphaRef)	discard;
		} else if (uAlphaFunc == EQUAL){
			if (gl_FragColor.w != uAlphaRef)	discard;
		} else if (uAlphaFunc == LEQUAL){
			if (gl_FragColor.w > uAlphaRef)		discard;
		} else if (uAlphaFunc == GREATER){
			if (gl_FragColor.w <= uAlphaRef)	discard;
		} else if (uAlphaFunc == NOTEQUAL){
			if (gl_FragColor.w == uAlphaRef)	discard;
		} else if (uAlphaFunc == GEQUAL){
			if (gl_FragColor.w < uAlphaRef)		discard;
		} else if (uAlphaFunc == ALWAYS){
		}
	}
}