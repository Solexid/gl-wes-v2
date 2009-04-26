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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wes.h"
#include "wes_uniform.h"

#ifndef __WES_SHADER_H__
#define __WES_SHADER_H__

/*  attribute bindings   */
#define WES_APOS                            0
#define WES_ANORMAL                         1
#define WES_AFOGCOORD                       2
#define WES_ACOLOR0                         3
#define WES_ACOLOR1                         4
#define WES_ATEXCOORD0                      5
#define WES_ATEXCOORD1                      6
#define WES_ATEXCOORD2                      7
#define WES_ATEXCOORD3                      8


/*  shader defines       */
#define WES_ALPHA_NEVER                     0
#define WES_ALPHA_LESS               		1
#define WES_ALPHA_EQUAL                     2
#define WES_ALPHA_LEQUAL                    3
#define WES_ALPHA_GREATER                   4
#define WES_ALPHA_NOTEQUAL                  5
#define WES_ALPHA_GEQUAL                    6
#define WES_ALPHA_ALWAYS                    7

#define WES_FUNC_NONE						0
#define WES_FUNC_REPLACE					1
#define WES_FUNC_MODULATE					2
#define WES_FUNC_ADD						3
#define WES_FUNC_ADD_SIGNED					4
#define WES_FUNC_INTERPOLATE				5
#define WES_FUNC_SUBTRACT					6
#define WES_FUNC_DOT3_RGB					7
#define WES_FUNC_DOT3_RGBA					8
#define WES_FUNC_DECAL						9
#define WES_FUNC_BLEND						10
#define WES_FUNC_COMBINE					11

#define WES_SRC_PREVIOUS					1
#define WES_SRC_CONSTANT					2
#define WES_SRC_PRIMARY_COLOR				3
#define WES_SRC_TEXTURE						4

#define WES_OP_ALPHA						1
#define WES_OP_ONE_MINUS_ALPHA				2
#define WES_OP_COLOR						3
#define WES_OP_ONE_MINUS_COLOR				4

typedef struct uniformloc_s     uniformloc_t;
typedef struct progstate_s      progstate_t;
typedef struct program_s        program_t;

struct uniformloc_s
{
    GLuint  uEnableRescaleNormal;
    GLuint  uEnableNormalize;
    GLuint  uEnableTextureGen[WES_MULTITEX_NUM];
    GLuint  uEnableClipPlane[WES_CLIPPLANE_NUM];
    GLuint  uEnableLighting;
    GLuint  uEnableLight[WES_LIGHT_NUM];
    GLuint  uEnableColorMaterial;
    GLuint  uEnableFog;
    GLuint  uEnableAlphaTest;
    GLuint  uEnableFogCoord;

    struct {
        GLuint  Position, ColorAmbient, ColorDiffuse, ColorSpec;
        GLuint  Attenuation, SpotDir;
        GLuint  SpotVar;
    } uLight[WES_LIGHT_NUM];

    struct {
        GLuint      ColorMaterial;
        GLuint      ColorAmbient, ColorDiffuse, ColorSpec, ColorEmissive;
        GLuint   	SpecExponent;
    } uMaterial[WES_FACE_NUM];

    struct {
    	GLuint  ColorAmbient;
        GLuint  TwoSided, LocalViewer;
        GLuint  ColorControl;
    } uLightModel;

    struct {
        GLuint  Func;		// {Tex function, RGB function, Alpha function}
        GLuint  Arg0;		// {RGB Src, RGB Op, Alpha Src, Alpha Op}
        GLuint  Arg1;		// {RGB Src, RGB Op, Alpha Src, Alpha Op}
        GLuint  Arg2;		// {RGB Src, RGB Op, Alpha Src, Alpha Op}
        GLuint  EnvColor;
        GLuint  Unit;
    } uTexture[WES_MULTITEX_NUM];

    GLuint  uRescaleFactor;

    GLuint      uFogMode;
    GLuint    uFogStart, uFogEnd, uFogDensity;
    GLuint     uFogColor;

    GLuint	uTexGenMode[WES_MULTITEX_NUM];
    GLuint		uTexGenMat[WES_MULTITEX_NUM];

    GLuint     uClipPlane[WES_CLIPPLANE_NUM];

    GLuint     uMVP;
    GLuint     uMV;
    GLuint     uMVIT;

    GLuint      uAlphaFunc;
    GLuint      uAlphaRef;
};

struct progstate_s
{
    GLuint  DEF_ALPHA_TEST;
    GLuint  DEF_TEXTURE0_ENV;
    GLuint  DEF_TEXTURE1_ENV;
    GLuint  DEF_TEXTURE2_ENV;
    GLuint  DEF_TEXTURE3_ENV;
    GLuint  DEF_FOG;
    GLuint  DEF_CLIPPLANE;
};

struct program_s
{
    GLuint          prog, frag, vert;
    uniformloc_t    uloc;
    progstate_t     pstate;
};

extern program_t    *sh_program;
extern GLboolean    sh_program_mod;
extern GLuint        sh_vertex;

extern GLvoid       wes_shader_init();
extern GLvoid       wes_shader_destroy();
extern GLvoid       wes_choose_program(progstate_t *s);
#endif
