//Defines:
#define LIGHT_NUM						8
#define CLIPPLANE_NUM					6

#define FACE_FRONT						0
#define FACE_BACK						1
#define FACE_NUM						2

#define COLORMAT_AMBIENT				0
#define COLORMAT_DIFFUSE				1
#define COLORMAT_AMBIENT_AND_DIFFUSE	2
#define COLORMAT_SPECULAR				3
#define COLORMAT_EMISSIVE				4

//Structures:
struct sLight {
	highp vec4 	Position;
	lowp vec4 	ColorAmbient, ColorDiffuse, ColorSpec;
	highp vec3 	Attenuation; 
	highp vec3	SpotDir;
	highp float SpotExponent, SpotCutoff;
};

struct sMaterial {
	int 		ColorMaterial;
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
uniform	bool			uEnableClipPlane[CLIPPLANE_NUM];
uniform	bool			uEnableLighting;
uniform	bool			uEnableLight[LIGHT_NUM];
uniform	bool			uEnableColorMaterial;
uniform	bool			uEnableFog;
uniform	bool			uEnableAlphaTest;
uniform	bool			uEnableFogCoord;

//Parameters:
uniform sampler2D		uTexture0, uTexture1;

uniform highp mat4			uMVP;		//model-view-projection matrix
uniform highp mat4			uMV;		//model-view matrix
uniform highp mat3			uMVIT;		//model-view inverted & transformed matrix 

uniform int				uFogType;
uniform float			uFogDensity, uFogStart, uFogEnd;
uniform lowp vec4		uFogColor;

uniform sLight			uLight[LIGHT_NUM];
uniform sLightModel		uLightModel;
uniform sMaterial		uMaterial[FACE_NUM];
uniform float 			uRescaleFactor;

//Clip plane equations:
uniform highp vec4 		uClipPlane[CLIPPLANE_NUM];

//Varyings:
varying lowp vec4 		vColor0;
varying lowp vec4 		vColor1;
varying mediump vec4 	vTexCoord0;
varying mediump vec4 	vTexCoord1;
varying lowp float 		vFogFactor;
varying lowp float 		vClipFactor;

//local variables:
highp vec4	lEye;
highp vec3	lNormal;
lowp vec4	lMaterialAmbient;
lowp vec4	lMaterialDiffuse;
lowp vec4	lMaterialSpecular;
lowp vec4	lMaterialEmissive;
float		lMaterialSpecExponent;
int			lFace;

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

vec4 ComputeLightFrom(int i){
	highp vec3 	dpos;
	highp vec4	col;
	float 	dist, dist2, spot;
	float 	att;
	float 	ndoth, ndotl;
	
	dpos = uLight[i].Position.xyz;
	att = 1.0;	

	if (uLight[i].Position.w != 0.0){
		dpos -= lEye.xyz;
		dist2 = dot(dpos, dpos);
		dist = sqrt(dist2);
		att = dot(uLight[i].Attenuation, vec3(1.0, dist, dist2));
		att = 1.0 / att;
					 
		dpos = normalize(dpos);
		if(uLight[i].SpotCutoff < 180.0){
			spot = dot(-dpos, uLight[i].SpotDir);
			if(spot >= cos(radians(uLight[i].SpotCutoff))){
				att *= pow(spot, uLight[i].SpotExponent);
			} else {
				return vec4(0,0,0,0);
			}
		}
	}
	ndotl = max(0.0, dot(lNormal, dpos));
	col = uLight[i].ColorAmbient * lMaterialAmbient;
	col += ndotl * uLight[i].ColorDiffuse * lMaterialDiffuse;
	dpos.z += 1.0;
	dpos = normalize(dpos);
	ndoth = max(0.0, dot(lNormal, dpos));
	col += pow(ndoth, lMaterialSpecExponent) * lMaterialSpecular * uLight[i].ColorSpec;
	
	att = clamp(att, 0.0,1.0);
	return (col * att);
}

vec4 ComputeLighting(){
	highp vec4 col;
	
	/* Normal in Eye Space*/
	lNormal = uMVIT * aNormal;
	//lNormal = (uMV * vec4(aNormal,0.0)).xyz;
		
	if (uEnableRescaleNormal){
		lNormal = uRescaleFactor * lNormal;
	}
	if (uEnableNormalize){
		lNormal = normalize(lNormal);
	}
		
	/* 	Determine Face 	*/
	if (uLightModel.TwoSided){
		if (lNormal.z > 0.0){
			lFace = FACE_FRONT;
		} else {
			lNormal = -lNormal;
			lFace = FACE_BACK;
		}	
	} else {
		lFace = FACE_FRONT;
	}
		
	/* Determine which materials are to be used	*/
	if (uEnableColorMaterial){
		if (uMaterial[lFace].ColorMaterial == COLORMAT_AMBIENT){
			lMaterialAmbient = aColor0;
			lMaterialDiffuse = uMaterial[lFace].ColorDiffuse;
			lMaterialSpecular = uMaterial[lFace].ColorSpec;
			lMaterialEmissive = uMaterial[lFace].ColorEmissive;
			lMaterialSpecExponent = uMaterial[lFace].SpecExponent;
		} else if (uMaterial[lFace].ColorMaterial == COLORMAT_DIFFUSE){
			lMaterialAmbient = uMaterial[lFace].ColorAmbient;
			lMaterialDiffuse = aColor0;
			lMaterialSpecular = uMaterial[lFace].ColorSpec;
			lMaterialEmissive = uMaterial[lFace].ColorEmissive;
			lMaterialSpecExponent = uMaterial[lFace].SpecExponent;			
		} else if (uMaterial[lFace].ColorMaterial == COLORMAT_AMBIENT_AND_DIFFUSE){
			lMaterialAmbient = aColor0;
			lMaterialDiffuse = aColor0;
			lMaterialSpecular = uMaterial[lFace].ColorSpec;
			lMaterialEmissive = uMaterial[lFace].ColorEmissive;
			lMaterialSpecExponent = uMaterial[lFace].SpecExponent;			
		} else if (uMaterial[lFace].ColorMaterial == COLORMAT_SPECULAR){
			lMaterialAmbient = uMaterial[lFace].ColorAmbient;
			lMaterialDiffuse = uMaterial[lFace].ColorDiffuse;
			lMaterialSpecular = aColor0;
			lMaterialEmissive = uMaterial[lFace].ColorEmissive;
			lMaterialSpecExponent = uMaterial[lFace].SpecExponent;			
		} else {
			lMaterialAmbient = uMaterial[lFace].ColorAmbient;
			lMaterialDiffuse = uMaterial[lFace].ColorDiffuse;
			lMaterialSpecular = uMaterial[lFace].ColorSpec;
			lMaterialEmissive =  aColor0;
			lMaterialSpecExponent = uMaterial[lFace].SpecExponent;			
		}
	} else {
		lMaterialAmbient = uMaterial[lFace].ColorAmbient;
		lMaterialDiffuse = uMaterial[lFace].ColorDiffuse;
		lMaterialSpecular = uMaterial[lFace].ColorSpec;
		lMaterialEmissive = uMaterial[lFace].ColorEmissive;
		lMaterialSpecExponent = uMaterial[lFace].SpecExponent;
	}
		
	col =  lMaterialEmissive;
	col += lMaterialAmbient * uLightModel.ColorAmbient;
	for(int i = 0; i < LIGHT_NUM; i++){
		if (uEnableLight[i]){
			col += ComputeLightFrom(i);
		}
	}
	col.w = lMaterialDiffuse.w;
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
		vColor0 = ComputeLighting();
	} else {
		vColor0 = aColor0;
	}
	
	vTexCoord0 = aTexCoord0;
	
	if (uEnableClipPlane[0]){
		vClipFactor = dot(lEye, uClipPlane[0]);
	}
}