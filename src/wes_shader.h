#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wes.h"

#ifndef __WES_SHADER_H__
#define __WES_SHADER_H__

//attribute indices:
#define WES_APOS             0
#define WES_ATEXCOORD0       1
#define WES_ATEXCOORD1       2
#define WES_ANORMAL          3
#define WES_AFOGCOORD        4
#define WES_ACOLOR0          5
#define WES_ACOLOR1          6
#define WES_ANUM             7

typedef struct sUniformLoc sUniformLoc;
struct sUniformLoc {
    //Enables:
    GLuint uEnableRescaleNormal;
    GLuint uEnableNormalize;
    GLuint uEnableTextureGenS;
    GLuint uEnableTextureGenT;
    GLuint uEnableTextureGenR;
    GLuint uEnableTextureGenQ;
    GLuint uEnableClipPlane[6];
    GLuint uEnableLighting;
    GLuint uEnableLight[8];
    GLuint uEnableColorMaterial;
    GLuint uEnableFog;
    GLuint uEnableAlphaTest;
    GLuint uEnableColor0;
    GLuint uEnableTexture0;
    GLuint uEnableFogCoord;

    GLuint uTexture0;

    struct {
        GLuint	Type;
        GLuint  DistAttenuation;
        GLuint 	Position;
        GLuint 	Attenuation[3];
        GLuint 	ColorAmbient, ColorDiffuse, ColorSpec;
        GLuint 	SpotDir, SpotExponent, SpotCutoff;
    } uLight[8];

    struct {
        GLuint 	ColorAmbient, ColorDiffuse, ColorSpec, ColorEmissive;
        GLuint 	SpecExponent;
    } uMaterial;

    GLuint uFogType, uFogDensity, uFogStart, uFogEnd, uFogColor;
    GLuint uClipPlane[6];

    GLuint uMVP;
    GLuint uMV;
    GLuint uMVinv;

    GLuint uAlphaFunc;
    GLuint uAlphaRef;
};

//variables
extern sUniformLoc sh_uloc;
extern GLuint   sh_program;
extern GLuint   sh_fragment;
extern GLuint   sh_vertex;

//functions:
extern GLvoid wes_shader_init();
extern GLvoid wes_shader_destroy();

#endif
