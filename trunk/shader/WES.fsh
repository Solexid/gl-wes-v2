/*
Copyright (C) 2009  Lachlan Tychsen - Smith aka Adventus

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


/* Defines to Enable and disable features	*/
//#define DEF_ALPHA_TEST
#define DEF_TEXTURE
//#define DEF_MULTI_TEXTURE
#define DEF_FOG
//#define DEF_CLIPPLANE

#define LIGHT_NUM						8
#define CLIPPLANE_NUM					6
#define MULTITEX_NUM					4
#define FACE_NUM						2

#define ALPHA_NEVER                     0
#define ALPHA_LESS               		1
#define ALPHA_EQUAL                     2
#define ALPHA_LEQUAL                    3
#define ALPHA_GREATER                   4
#define ALPHA_NOTEQUAL                  5
#define ALPHA_GEQUAL                    6
#define ALPHA_ALWAYS                    7

#define FUNC_NONE						0
#define FUNC_REPLACE					1
#define FUNC_MODULATE					2
#define FUNC_ADD						3
#define FUNC_ADD_SIGNED					4
#define FUNC_INTERPOLATE				5
#define FUNC_SUBTRACT					6
#define FUNC_DOT3_RGB					7
#define FUNC_DOT3_RGBA					8
#define FUNC_DECAL						9
#define FUNC_BLEND						10
#define FUNC_COMBINE					11

#define SRC_PREVIOUS					1
#define SRC_CONSTANT					2
#define SRC_PRIMARY_COLOR				3
#define SRC_TEXTURE						4

#define OP_ALPHA						1
#define OP_ONE_MINUS_ALPHA				2
#define OP_COLOR						3
#define OP_ONE_MINUS_COLOR				4

struct sMultiTexture {
	ivec3 Func;		// {Tex function, RGB function, Alpha function}
	ivec4 Arg[3];	// {RGB Src, RGB Op, Alpha Src, Alpha Op}
	lowp vec4 EnvColor;
	sampler2D Unit;
};

uniform lowp vec4		uFogColor;
uniform sMultiTexture	uTexture[MULTITEX_NUM]; 

uniform	bool			uEnableAlphaTest;
uniform	int				uAlphaFunc;
uniform	highp float		uAlphaRef;

uniform bool			uEnableMultiTexture;
uniform	bool			uEnableFog;
uniform	bool			uEnableClipPlane[CLIPPLANE_NUM];

//Varyings:
varying lowp vec4 		vColor;
varying lowp vec2 		vFactor;
#ifdef DEF_MULTI_TEXTURE
varying mediump vec4 	vTexCoord[MULTITEX_NUM];
#else
varying mediump vec4 	vTexCoord[1];
#endif

#ifdef DEF_MULTI_TEXTURE
lowp vec4 ObtainArg(int i, int a){
	lowp vec4 res;
	
	/* 	Get RGB Component	*/
	if (uTexture[i].Arg[a].x == SRC_PREVIOUS){
		res.rgb = gl_FragColor.rgb;
	} else if (uTexture[i].Arg[a].x == SRC_CONSTANT){
		res.rgb = uTexture[i].EnvColor.rgb;
	} else if (uTexture[i].Arg[a].x == SRC_PRIMARY_COLOR){
		res.rgb = vColor.rgb;
	} else if (uTexture[i].Arg[a].x == SRC_TEXTURE){
		res.rgb = texture2D(uTexture[i].Unit, vTexCoord[i].xy).rgb;
	}
		
	/* 	Get Alpha Component	*/
	if (uTexture[i].Arg[a].z == SRC_PREVIOUS){
		res.a = gl_FragColor.a;
	} else if (uTexture[i].Arg[a].z == SRC_CONSTANT){
		res.a = uTexture[i].EnvColor.a;
	} else if (uTexture[i].Arg[a].z == SRC_PRIMARY_COLOR){
		res.a = vColor.a;
	}else if (uTexture[i].Arg[a].z == SRC_TEXTURE){
		res.a = texture2D(uTexture[i].Unit, vTexCoord[i].xy).a;
	}
	
	if (uTexture[i].Arg[a].w != OP_ALPHA){
		res.a = 1.0 - res.a;
	}
	
	if (uTexture[i].Arg[a].y == OP_ALPHA){
		res.rgb = vec3(res.a, res.a, res.a);	
	} else if (uTexture[i].Arg[a].y == OP_ONE_MINUS_ALPHA){
		lowp float one_a = 1.0 - res.a;
		res.rgb = vec3(one_a, one_a, one_a);	
	} else if (uTexture[i].Arg[a].y == OP_ONE_MINUS_COLOR){
		res.rgb = vec3(1.0,1.0,1.0) - res.rgb;
	}
	
	return res;
}

void ApplyTexCombine(int i){
	lowp vec4 Arg0, Arg1, Arg2;
	Arg0 = ObtainArg(i, 0);
	if (uTexture[i].Func.y != FUNC_REPLACE || uTexture[i].Func.z != FUNC_REPLACE){
		Arg1 = ObtainArg(i, 1);
	}
	if (uTexture[i].Func.y == FUNC_INTERPOLATE || uTexture[i].Func.z == FUNC_INTERPOLATE){
		Arg2 = ObtainArg(i, 2);	
	}
	if (uTexture[i].Func.y == FUNC_DOT3_RGBA){		
		Arg0.rgb -= vec3(0.5, 0.5, 0.5);
		Arg1.rgb -= vec3(0.5, 0.5, 0.5);
		lowp float c = 4.0 * dot(Arg0.rgb, Arg1.rgb);
		gl_FragColor = vec4(c, c, c, c);
		return;		
	} else {
		/*	RGB Component	*/
		if (uTexture[i].Func.y == FUNC_REPLACE){		
			gl_FragColor.rgb = Arg0.rgb; 	
		} else if (uTexture[i].Func.y == FUNC_MODULATE){		
			gl_FragColor.rgb = Arg0.rgb * Arg1.rgb;		
		} else if (uTexture[i].Func.y == FUNC_ADD){		
			gl_FragColor.rgb = Arg0.rgb + Arg1.rgb;		
		} else if (uTexture[i].Func.y == FUNC_ADD_SIGNED){		
			gl_FragColor.rgb = Arg0.rgb + Arg1.rgb - vec3(0.5,0.5,0.5);		
		} else if (uTexture[i].Func.y == FUNC_INTERPOLATE){		
			gl_FragColor.rgb = Arg0.rgb * Arg2.rgb + 
							   Arg1.rgb * (vec3(1.0,1.0,1.0) - Arg2.rgb);		
		} else if (uTexture[i].Func.y == FUNC_SUBTRACT){		
			gl_FragColor.rgb = Arg0.rgb - Arg1.rgb;
		} else if (uTexture[i].Func.y == FUNC_DOT3_RGBA){		
			Arg0.rgb -= vec3(0.5, 0.5, 0.5);
			Arg1.rgb -= vec3(0.5, 0.5, 0.5);
			lowp float c = 4.0 * dot(Arg0.rgb, Arg1.rgb);
			gl_FragColor.rgb = vec3(c, c, c);
		}
		
		/*	Alpha Component	*/
		if (uTexture[i].Func.z == FUNC_REPLACE){		
			gl_FragColor.a = Arg0.a;		
		} else if (uTexture[i].Func.z == FUNC_MODULATE){		
			gl_FragColor.a = Arg0.a * Arg1.a;		
		} else if (uTexture[i].Func.z == FUNC_ADD){		
			gl_FragColor.a = Arg0.a + Arg1.a;		
		} else if (uTexture[i].Func.z == FUNC_ADD_SIGNED){		
			gl_FragColor.a = Arg0.a + Arg1.a - 0.5;		
		} else if (uTexture[i].Func.z == FUNC_INTERPOLATE){		
			gl_FragColor.a = Arg0.a * Arg2.a + Arg1.a * (1.0 - Arg2.a);		
		} else if (uTexture[i].Func.z == FUNC_SUBTRACT){		
			gl_FragColor.a = Arg0.a - Arg1.a;
		}
		return;
	}
}

void ApplyTex(int i){

	if (uTexture[i].Func.x == FUNC_REPLACE){
		gl_FragColor = texture2D(uTexture[i].Unit, vTexCoord[i].xy);		
	} else if (uTexture[i].Func.x == FUNC_MODULATE){
		gl_FragColor *= texture2D(uTexture[i].Unit, vTexCoord[i].xy);		
	} else if (uTexture[i].Func.x == FUNC_DECAL){	
		lowp vec4 col = texture2D(uTexture[i].Unit, vTexCoord[i].xy);
		gl_FragColor.rgb = (1.0 - col.a) * gl_FragColor.rgb + col.a * col.rgb;		
	} else if (uTexture[i].Func.x == FUNC_BLEND){
		lowp vec4 col = texture2D(uTexture[i].Unit, vTexCoord[i].xy);
		gl_FragColor.rgb = (vec3(1.0,1.0,1.0) - col.rgb) * gl_FragColor.rgb + 
							col.rgb * uTexture[i].EnvColor.rgb;	
		gl_FragColor.a *= col.a;
	} else if (uTexture[i].Func.x == FUNC_ADD){
		lowp vec4 col = texture2D(uTexture[i].Unit, vTexCoord[i].xy);
		gl_FragColor.rgb += col.rgb;	
		gl_FragColor.a 	 *= col.a;	
	} else if (uTexture[i].Func.x == FUNC_COMBINE){
		ApplyTexCombine(i);
	}

}
#endif


void main(){
	int i;


#ifdef DEF_TEXTURE
#ifdef DEF_MULTI_TEXTURE
	gl_FragColor = vColor;	
	for(i = 0; i < MULTITEX_NUM; i++){
		if (uTexture[i].Func.x != FUNC_NONE){
			ApplyTex(i);
		}
	};
#else
	gl_FragColor = vColor * texture2D(uTexture[0].Unit, vTexCoord[0].xy);
#endif	//MULTI_TEXTURE
#else
	gl_FragColor = vColor;
#endif	//TEXTURE

#ifdef DEF_FOG
	if (uEnableFog){
		gl_FragColor = gl_FragColor * vFactor.x + (1.0 - vFactor.x) * uFogColor;
	}
#endif

#ifdef DEF_CLIPPLANE
	gl_FragColor.w *= vFactor.y;
#endif

#ifdef DEF_ALPHA_TEST
	if (uEnableAlphaTest){
		if (uAlphaFunc == ALPHA_NEVER){		discard;
		} else if (uAlphaFunc == ALPHA_LESS){
			if (gl_FragColor.w >= uAlphaRef)	discard;
		} else if (uAlphaFunc == ALPHA_EQUAL){
			if (gl_FragColor.w != uAlphaRef)	discard;
		} else if (uAlphaFunc == ALPHA_LEQUAL){
			if (gl_FragColor.w > uAlphaRef)		discard;
		} else if (uAlphaFunc == ALPHA_GREATER){
			if (gl_FragColor.w <= uAlphaRef)	discard;
		} else if (uAlphaFunc == ALPHA_NOTEQUAL){
			if (gl_FragColor.w == uAlphaRef)	discard;
		} else if (uAlphaFunc == ALPHA_GEQUAL){
			if (gl_FragColor.w < uAlphaRef)		discard;
		} else if (uAlphaFunc == ALPHA_ALWAYS){
		}
	}
#endif
}