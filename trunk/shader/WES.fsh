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

#define LIGHT_NUM						8
#define CLIPPLANE_NUM					6
#define MULTITEX_NUM					4
#define FACE_NUM						2

#define ALPHA_NEVER                     1
#define ALPHA_LESS               		2
#define ALPHA_EQUAL                     3
#define ALPHA_LEQUAL                    4
#define ALPHA_GREATER                   5
#define ALPHA_NOTEQUAL                  6
#define ALPHA_GEQUAL                    7
#define ALPHA_ALWAYS                    8

#define FUNC_NONE						0
#define FUNC_REPLACE					1
#define FUNC_MODULATE					2
#define FUNC_ADD						3
#define FUNC_DECAL						4
#define FUNC_BLEND						5
#define FUNC_COMBINE					6

#define FUNC_ADD_SIGNED					7
#define FUNC_INTERPOLATE				8
#define FUNC_SUBTRACT					9
#define FUNC_DOT3_RGB					10
#define FUNC_DOT3_RGBA					11


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
uniform	highp float		uAlphaRef;

//Varyings:
varying lowp vec4 		vColor;
varying lowp vec2 		vFactor;
#ifdef DEF_MULTI_TEXTURE
varying mediump vec4 	vTexCoord[MULTITEX_NUM];
#else
varying mediump vec4 	vTexCoord[MULTITEX_NUM];
#endif

#ifdef DEF_MULTI_TEXTURE
lowp vec4 ObtainArg(int i, int a){
	lowp vec4 res;
	
	/* 	Get RGB Component	*/
	if (uTexture[3].Arg[a].x == SRC_PREVIOUS){
		res.rgb = gl_FragColor.rgb;
	} else if (uTexture[3].Arg[a].x == SRC_CONSTANT){
		res.rgb = uTexture[3].EnvColor.rgb;
	} else if (uTexture[3].Arg[a].x == SRC_PRIMARY_COLOR){
		res.rgb = vColor.rgb;
	} else if (uTexture[3].Arg[a].x == SRC_TEXTURE){
		res.rgb = texture2D(uTexture[3].Unit, vTexCoord[3].xy).rgb;
	}
		
	/* 	Get Alpha Component	*/
	if (uTexture[3].Arg[a].z == SRC_PREVIOUS){
		res.a = gl_FragColor.a;
	} else if (uTexture[3].Arg[a].z == SRC_CONSTANT){
		res.a = uTexture[3].EnvColor.a;
	} else if (uTexture[3].Arg[a].z == SRC_PRIMARY_COLOR){
		res.a = vColor.a;
	}else if (uTexture[3].Arg[a].z == SRC_TEXTURE){
		res.a = texture2D(uTexture[3].Unit, vTexCoord[3].xy).a;
	}
	
	if (uTexture[3].Arg[a].w != OP_ALPHA){
		res.a = 1.0 - res.a;
	}
	
	if (uTexture[3].Arg[a].y == OP_ALPHA){
		res.rgb = vec3(res.a, res.a, res.a);	
	} else if (uTexture[3].Arg[a].y == OP_ONE_MINUS_ALPHA){
		lowp float one_a = 1.0 - res.a;
		res.rgb = vec3(one_a, one_a, one_a);	
	} else if (uTexture[3].Arg[a].y == OP_ONE_MINUS_COLOR){
		res.rgb = vec3(1.0,1.0,1.0) - res.rgb;
	}
	
	return res;
}

void ApplyTexCombine(int i){
	lowp vec4 Arg0, Arg1, Arg2;
	Arg0 = ObtainArg(i, 0);
	if (uTexture[3].Func.y != FUNC_REPLACE || uTexture[3].Func.z != FUNC_REPLACE){
		Arg1 = ObtainArg(i, 1);
	}
	if (uTexture[3].Func.y == FUNC_INTERPOLATE || uTexture[3].Func.z == FUNC_INTERPOLATE){
		Arg2 = ObtainArg(i, 2);	
	}
	if (uTexture[3].Func.y == FUNC_DOT3_RGBA){		
		Arg0.rgb -= vec3(0.5, 0.5, 0.5);
		Arg1.rgb -= vec3(0.5, 0.5, 0.5);
		lowp float c = 4.0 * dot(Arg0.rgb, Arg1.rgb);
		gl_FragColor = vec4(c, c, c, c);
		return;		
	} else {
		/*	RGB Component	*/
		if (uTexture[3].Func.y == FUNC_REPLACE){		
			gl_FragColor.rgb = Arg0.rgb; 	
		} else if (uTexture[3].Func.y == FUNC_MODULATE){		
			gl_FragColor.rgb = Arg0.rgb * Arg1.rgb;		
		} else if (uTexture[3].Func.y == FUNC_ADD){		
			gl_FragColor.rgb = Arg0.rgb + Arg1.rgb;		
		} else if (uTexture[3].Func.y == FUNC_ADD_SIGNED){		
			gl_FragColor.rgb = Arg0.rgb + Arg1.rgb - vec3(0.5,0.5,0.5);		
		} else if (uTexture[3].Func.y == FUNC_INTERPOLATE){		
			gl_FragColor.rgb = Arg0.rgb * Arg2.rgb + 
							   Arg1.rgb * (vec3(1.0,1.0,1.0) - Arg2.rgb);		
		} else if (uTexture[3].Func.y == FUNC_SUBTRACT){		
			gl_FragColor.rgb = Arg0.rgb - Arg1.rgb;
		} else if (uTexture[3].Func.y == FUNC_DOT3_RGBA){		
			Arg0.rgb -= vec3(0.5, 0.5, 0.5);
			Arg1.rgb -= vec3(0.5, 0.5, 0.5);
			lowp float c = 4.0 * dot(Arg0.rgb, Arg1.rgb);
			gl_FragColor.rgb = vec3(c, c, c);
		}
		
		/*	Alpha Component	*/
		if (uTexture[3].Func.z == FUNC_REPLACE){		
			gl_FragColor.a = Arg0.a;		
		} else if (uTexture[3].Func.z == FUNC_MODULATE){		
			gl_FragColor.a = Arg0.a * Arg1.a;		
		} else if (uTexture[3].Func.z == FUNC_ADD){		
			gl_FragColor.a = Arg0.a + Arg1.a;		
		} else if (uTexture[3].Func.z == FUNC_ADD_SIGNED){		
			gl_FragColor.a = Arg0.a + Arg1.a - 0.5;		
		} else if (uTexture[3].Func.z == FUNC_INTERPOLATE){		
			gl_FragColor.a = Arg0.a * Arg2.a + Arg1.a * (1.0 - Arg2.a);		
		} else if (uTexture[3].Func.z == FUNC_SUBTRACT){		
			gl_FragColor.a = Arg0.a - Arg1.a;
		}
		return;
	}
}
#endif

void ApplyTex(){
/* 	Texture 0 	*/
#if DEF_TEXTURE0_ENV == FUNC_REPLACE
	gl_FragColor = texture2D(uTexture[0].Unit, vTexCoord[0].xy);		
#elif DEF_TEXTURE0_ENV == FUNC_MODULATE
	gl_FragColor *= texture2D(uTexture[0].Unit, vTexCoord[0].xy);		
#elif DEF_TEXTURE0_ENV == FUNC_DECAL
	lowp vec4 col = texture2D(uTexture[0].Unit, vTexCoord[0].xy);
	gl_FragColor.rgb = (1.0 - col.a) * gl_FragColor.rgb + col.a * col.rgb;		
#elif DEF_TEXTURE0_ENV == FUNC_BLEND
	lowp vec4 col = texture2D(uTexture[0].Unit, vTexCoord[0].xy);
	gl_FragColor.rgb = (vec3(1.0,1.0,1.0) - col.rgb) * gl_FragColor.rgb + 
							col.rgb * uTexture[0].EnvColor.rgb;	
	gl_FragColor.a *= col.a;
#elif DEF_TEXTURE0_ENV == FUNC_ADD
	lowp vec4 col = texture2D(uTexture[0].Unit, vTexCoord[0].xy);
	gl_FragColor.rgb += col.rgb;	
	gl_FragColor.a 	 *= col.a;	
#elif DEF_TEXTURE0_ENV == FUNC_COMBINE
	ApplyTexCombine(0);
#endif	

/* 	Texture 1 	*/
#if DEF_TEXTURE1_ENV == FUNC_REPLACE
	gl_FragColor = texture2D(uTexture[1].Unit, vTexCoord[1].xy);		
#elif DEF_TEXTURE1_ENV == FUNC_MODULATE
	gl_FragColor *= texture2D(uTexture[1].Unit, vTexCoord[1].xy);		
#elif DEF_TEXTURE1_ENV == FUNC_DECAL
	lowp vec4 col = texture2D(uTexture[1].Unit, vTexCoord[1].xy);
	gl_FragColor.rgb = (1.0 - col.a) * gl_FragColor.rgb + col.a * col.rgb;		
#elif DEF_TEXTURE1_ENV == FUNC_BLEND
	lowp vec4 col = texture2D(uTexture[1].Unit, vTexCoord[1].xy);
	gl_FragColor.rgb = (vec3(1.0,1.0,1.0) - col.rgb) * gl_FragColor.rgb + 
							col.rgb * uTexture[1].EnvColor.rgb;	
	gl_FragColor.a *= col.a;
#elif DEF_TEXTURE1_ENV == FUNC_ADD
	lowp vec4 col = texture2D(uTexture[1].Unit, vTexCoord[1].xy);
	gl_FragColor.rgb += col.rgb;	
	gl_FragColor.a 	 *= col.a;	
#elif DEF_TEXTURE1_ENV == FUNC_COMBINE
	ApplyTexCombine(1);
#endif	

/* 	Texture 2 	*/
#if DEF_TEXTURE2_ENV == FUNC_REPLACE
	gl_FragColor = texture2D(uTexture[2].Unit, vTexCoord[2].xy);		
#elif DEF_TEXTURE2_ENV == FUNC_MODULATE
	gl_FragColor *= texture2D(uTexture[2].Unit, vTexCoord[2].xy);		
#elif DEF_TEXTURE2_ENV == FUNC_DECAL
	lowp vec4 col = texture2D(uTexture[2].Unit, vTexCoord[2].xy);
	gl_FragColor.rgb = (1.0 - col.a) * gl_FragColor.rgb + col.a * col.rgb;		
#elif DEF_TEXTURE2_ENV == FUNC_BLEND
	lowp vec4 col = texture2D(uTexture[2].Unit, vTexCoord[2].xy);
	gl_FragColor.rgb = (vec3(1.0,1.0,1.0) - col.rgb) * gl_FragColor.rgb + 
							col.rgb * uTexture[2].EnvColor.rgb;	
	gl_FragColor.a *= col.a;
#elif DEF_TEXTURE2_ENV == FUNC_ADD
	lowp vec4 col = texture2D(uTexture[2].Unit, vTexCoord[2].xy);
	gl_FragColor.rgb += col.rgb;	
	gl_FragColor.a 	 *= col.a;	
#elif DEF_TEXTURE2_ENV == FUNC_COMBINE
	ApplyTexCombine(2);
#endif	

/* 	Texture 3 	*/
#if DEF_TEXTURE3_ENV == FUNC_REPLACE
	gl_FragColor = texture2D(uTexture[3].Unit, vTexCoord[3].xy);		
#elif DEF_TEXTURE3_ENV == FUNC_MODULATE
	gl_FragColor *= texture2D(uTexture[3].Unit, vTexCoord[3].xy);		
#elif DEF_TEXTURE3_ENV == FUNC_DECAL
	lowp vec4 col = texture2D(uTexture[3].Unit, vTexCoord[3].xy);
	gl_FragColor.rgb = (1.0 - col.a) * gl_FragColor.rgb + col.a * col.rgb;		
#elif DEF_TEXTURE3_ENV == FUNC_BLEND
	lowp vec4 col = texture2D(uTexture[3].Unit, vTexCoord[3].xy);
	gl_FragColor.rgb = (vec3(1.0,1.0,1.0) - col.rgb) * gl_FragColor.rgb + 
							col.rgb * uTexture[3].EnvColor.rgb;	
	gl_FragColor.a *= col.a;
#elif DEF_TEXTURE3_ENV == FUNC_ADD
	lowp vec4 col = texture2D(uTexture[3].Unit, vTexCoord[3].xy);
	gl_FragColor.rgb += col.rgb;	
	gl_FragColor.a 	 *= col.a;	
#elif DEF_TEXTURE3_ENV == FUNC_COMBINE
	ApplyTexCombine(3);
#endif	

}

void ApplyAlphaTest(){
#if DEF_ALPHA_TEST == ALPHA_NEVER		
	discard;
#elif DEF_ALPHA_TEST == ALPHA_LESS
	if (gl_FragColor.w >= uAlphaRef)	discard;
#elif DEF_ALPHA_TEST == ALPHA_EQUAL
	if (gl_FragColor.w != uAlphaRef)	discard;
#elif DEF_ALPHA_TEST == ALPHA_LEQUAL
	if (gl_FragColor.w > uAlphaRef)		discard;
#elif DEF_ALPHA_TEST == ALPHA_GREATER
	if (gl_FragColor.w <= uAlphaRef)	discard;
#elif DEF_ALPHA_TEST == ALPHA_NOTEQUAL
	if (gl_FragColor.w == uAlphaRef)	discard;
#elif DEF_ALPHA_TEST == ALPHA_GEQUAL
	if (gl_FragColor.w < uAlphaRef)		discard;
#endif
}

void main(){
	int i;

	gl_FragColor = vColor;	
	ApplyTex();

#if DEF_FOG == 1
	gl_FragColor = gl_FragColor * vFactor.x + (1.0 - vFactor.x) * uFogColor;
#endif

#if DEF_CLIPPLANE == 1
	gl_FragColor.w *= vFactor.y;
#endif

	ApplyAlphaTest();

}