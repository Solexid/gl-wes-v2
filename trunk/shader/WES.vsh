//Defines:
#define	LIGHT_TYPE_NONE 	0
#define	LIGHT_TYPE_DIR 		1
#define	LIGHT_TYPE_POINT	2
#define	LIGHT_TYPE_SPOT		3


#define LIGHT_NUM			8

//Structures:
struct sLight {
	int			Type;
	bool 		DistAttenuation;
	highp vec4 	Position;
	highp float	Attenuation[3];
	lowp vec4 	ColorAmbient, ColorDiffuse, ColorSpec;
	
	highp vec3 	SpotDir;
	float 		SpotExponent, SpotCutoff;
};

struct sMaterial {
	lowp vec4 	ColorAmbient, ColorDiffuse, ColorSpec, ColorEmissive;
	float 		SpecExponent;
};

struct sLightModel {
	lowp vec4 	ColorAmbient;
	bool		TwoSided;	
	bool		LocalViewer;
	int			ColorControl;		
};

//Attributes:
attribute highp vec4 	aPosition;
attribute mediump vec4 	aTexCoord0;
attribute mediump vec4 	aTexCoord1;
attribute highp vec3 	aNormal;
attribute highp float 	aFogCoord;
attribute lowp vec4 	aColor0;
attribute lowp vec4 	aColor1;

//Enables:
uniform	bool			uEnableRescaleNormal;
uniform	bool			uEnableNormalize;
uniform	bool			uEnableTextureGenS;
uniform	bool			uEnableTextureGenT;
uniform	bool			uEnableTextureGenR;
uniform	bool			uEnableTextureGenQ;
uniform	bool			uEnableClipPlane[6];
uniform	bool			uEnableLighting;
uniform	bool			uEnableLight[LIGHT_NUM];
uniform	bool			uEnableColorMaterial;
uniform	bool			uEnableFog;
uniform	bool			uEnableAlphaTest;
uniform	bool			uEnableFogCoord;

uniform	bool			uEnableColor0;
uniform	bool			uEnableTexture0;

//Parameters:
uniform sampler2D		uTexture0, uTexture1;

uniform mat4			uMVP;		//model-view-projection matrix
uniform mat4			uMV;		//model-view matrix
uniform mat3			uMVIT;		//model-view inverted & transformed matrix 

uniform int				uFogType;
uniform float			uFogDensity, uFogStart, uFogEnd;
uniform lowp vec4		uFogColor;

uniform sLight			uLight[LIGHT_NUM];
uniform sLightModel		uLightModel;
uniform sMaterial		uMaterial[2];
uniform float 			uRescaleFactor;

//Clip plane equations:
uniform highp vec4 		uClipPlane[6];

//Varyings:
varying lowp vec4 		vColor0;
varying lowp vec4 		vColor1;
varying mediump vec4 	vTexCoord0;
varying mediump vec4 	vTexCoord1;
varying mediump float 	vFogFactor;
varying mediump float 	vClipFactor;

//local variables:
highp vec4	lEye;
highp vec3	lNormal;

float ComputeFog(float dist){
	highp float f;
	if(uFogType == 0){
		f = (uFogEnd - dist) / (uFogEnd - uFogStart);
	} else if(uFogType == 1){
		f = exp(-(dist * uFogDensity));
	} else {
		f = dist * uFogDensity;
		f = exp(-(f * f));
	}
	return clamp(f, 0.0, 1.0);
}

vec4 ComputeLightFrom(int i, int mat){
	vec3 	dpos;
	float 	att = 1.0;
	vec4	col;
	
	if (uLight[i].Position.w != 0.0){
		//Point / Spot light
		dpos = uLight[i].Position.xyz - lEye.xyz;
		if (uLight[i].DistAttenuation){
			float attdist, attdist2;
			attdist2 = dot(dpos, dpos);
			attdist = sqrt(attdist2);
			att = 1.0 / (uLight[i].Attenuation[0] + 
						uLight[i].Attenuation[1] * attdist + 
						uLight[i].Attenuation[2] * attdist2);
		}
		dpos = normalize(dpos);
		
		if (uLight[i].Type == LIGHT_TYPE_SPOT){
			//Spot Light:
			if(uLight[i].SpotCutoff < 180.0){
				float f = dot(-dpos, uLight[i].SpotDir);
				if(f >= cos(radians(uLight[i].SpotCutoff))){
					att *= pow(f, uLight[i].SpotExponent);
				} else {
					return vec4(0,0,0,0);
				}
			}
		}
	} else {
		//Directional Light
		dpos = uLight[i].Position.xyz;
	}

	float ndotl = max(0.0, dot(lNormal, dpos));	
	if (uEnableColorMaterial){
		col = (uLight[i].ColorAmbient * uMaterial[mat].ColorAmbient);
		col += ndotl * (uLight[i].ColorDiffuse * uMaterial[mat].ColorDiffuse);
		dpos.z += 1.0;
		dpos = normalize(dpos);
		float ndoth = max(0.0, dot(lNormal, dpos));
		col += pow(ndoth, uMaterial[mat].SpecExponent) * (uMaterial[mat].ColorSpec * uLight[i].ColorSpec);
	} else {
		col = uLight[i].ColorAmbient;
		col += ndotl * (uLight[i].ColorDiffuse);
		dpos.z += 1.0;
		dpos = normalize(dpos);
		float ndoth = max(0.0, dot(lNormal, dpos));
		col += ndoth * (uLight[i].ColorSpec);
	}
	return (col * att);
}

vec4 ComputeLighting(int mat){
	vec4 col;
	if (uEnableColorMaterial){
	 	col = uMaterial[mat].ColorEmissive + uMaterial[mat].ColorAmbient * uLightModel.ColorAmbient;
	} else {
	 	col = uLightModel.ColorAmbient;
	}
	for(int i = 0; i < 8; i++){
		if (uEnableLight[i]){
			col += ComputeLightFrom(i, mat);
		}
	}
	if (uEnableColorMaterial){
		col.w = uMaterial[mat].ColorDiffuse.w;
	}
	return col;
}

void main(){
	gl_Position = uMVP * aPosition;

	if ((uEnableFog && !uEnableFogCoord) || uEnableLighting || uEnableClipPlane[0]){
		lEye = uMV * aPosition;
	}
	
	if (uEnableFog){
		if (uEnableFogCoord){
			vFogFactor = ComputeFog(aFogCoord);			
		}else{
			vFogFactor = ComputeFog(-lEye.z);
		}	
	}	

	if (uEnableLighting){
		lNormal = uMVIT * aNormal;
		
		if (uEnableRescaleNormal){
			lNormal = uRescaleFactor * lNormal;
		}
		if (uEnableNormalize){
			lNormal = normalize(lNormal);
		}
		
		if (uLightModel.TwoSided){
			if (lNormal.z > 0.0){
				vColor0 = ComputeLighting(0);
			} else {
				lNormal = -lNormal;
				vColor0 = ComputeLighting(1);
			}			
		} else {
			lNormal.z = abs(lNormal.z);
			vColor0 = ComputeLighting(0);
		}
	}

	if (uEnableColor0){
		vColor0 = aColor0;
	}
	
	if (uEnableTexture0){
		vTexCoord0 = aTexCoord0;
	}
	
	if (uEnableClipPlane[0]){
		vClipFactor = dot(lEye, uClipPlane[0]);
	}
}