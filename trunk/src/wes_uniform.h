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

#ifndef __WES_UNIFORM_H__
#define __WES_UNIFORM_H__

#define WES_LIGHT_NUM           8
#define WES_CLIPPLANE_NUM       6
#define WES_MULTITEX_NUM        4
#define WES_FACE_NUM            2

typedef struct uvec4_s      uvec4_t;
typedef struct uvec3_s      uvec3_t;
typedef struct uvec2_s      uvec2_t;
typedef struct uivec4_s     ubvec4_t;
typedef struct uivec3_s     ubvec3_t;
typedef struct uivec2_s     ubvec2_t;
typedef struct uivec4_s     uivec4_t;
typedef struct uivec3_s     uivec3_t;
typedef struct uivec2_s     uivec2_t;
typedef struct ufloat_s     ufloat_t;
typedef struct uint_s       usampler2d_t;
typedef struct uint_s       uint_t;
typedef struct uint_s       ubool_t;
typedef struct umat2_s      umat2_t;
typedef struct umat3_s      umat3_t;
typedef struct umat4_s      umat4_t;
typedef struct uniforms_s   uniforms_t;

/* Uniform types */
struct uvec4_s{
    GLboolean   mod;
    GLuint      loc;
    union {
        GLfloat v[4];
        struct {
            GLfloat x, y, z, w;
        };
    };
};

struct uvec3_s{
    GLboolean   mod;
    GLuint      loc;
    union {
        GLfloat v[3];
        struct {
            GLfloat x, y, z;
        };
    };
};

struct uvec2_s{
    GLboolean   mod;
    GLuint      loc;
    union {
        GLfloat v[2];
        struct {
            GLfloat x, y;
        };
    };
};

struct uivec4_s{
    GLboolean   mod;
    GLuint      loc;
    union {
        GLint v[4];
        struct {
            GLint x, y, z, w;
        };
    };
};

struct uivec3_s{
    GLboolean   mod;
    GLuint      loc;
    union {
        GLint v[3];
        struct {
            GLint x, y, z;
        };
    };
};

struct uivec2_s{
    GLboolean   mod;
    GLuint      loc;
    union {
        GLint v[2];
        struct {
            GLint x, y;
        };
    };
};

struct ufloat_s{
    GLboolean   mod;
    GLuint  loc;
    GLfloat f;
};

struct uint_s{
    GLboolean   mod;
    GLuint  loc;
    GLint   i;
};

struct umat2_s{
    GLuint  loc;
};

struct umat3_s{
    GLuint  loc;
};

struct umat4_s{
    GLuint  loc;
};

struct uniforms_s {
    //Enables:
    ubool_t     uEnableRescaleNormal;
    ubool_t     uEnableNormalize;
    ubvec4_t    uEnableTextureGen[WES_MULTITEX_NUM];
    ubool_t     uEnableClipPlane[WES_CLIPPLANE_NUM];
    ubool_t     uEnableLighting;
    ubool_t     uEnableLight[WES_LIGHT_NUM];
    ubool_t     uEnableColorMaterial;
    ubool_t     uEnableFog;
    ubool_t     uEnableAlphaTest;
    ubool_t     uEnableFogCoord;

    struct {
        uvec4_t     Position, ColorAmbient, ColorDiffuse, ColorSpec;
        uvec3_t     Attenuation, SpotDir;
        uvec2_t     SpotVar;
    } uLight[WES_LIGHT_NUM];

    struct {
        uint_t      ColorMaterial;
        uvec4_t     ColorAmbient, ColorDiffuse, ColorSpec, ColorEmissive;
        ufloat_t 	SpecExponent;
    } uMaterial[WES_FACE_NUM];

    struct {
    	uvec4_t     ColorAmbient;
        ubool_t     TwoSided, LocalViewer;
        uint_t      ColorControl;
    } uLightModel;

    struct {
        uivec3_t        Func;		// {Tex function, RGB function, Alpha function}
        uivec4_t        Arg0;		// {RGB Src, RGB Op, Alpha Src, Alpha Op}
        uivec4_t        Arg1;		// {RGB Src, RGB Op, Alpha Src, Alpha Op}
        uivec4_t        Arg2;		// {RGB Src, RGB Op, Alpha Src, Alpha Op}
        uvec4_t         EnvColor;
        usampler2d_t    Unit;
    } uTexture[WES_MULTITEX_NUM];

    ufloat_t    uRescaleFactor;

    uint_t      uFogMode;
    ufloat_t    uFogStart, uFogEnd, uFogDensity;
    uvec4_t     uFogColor;

    uivec4_t	uTexGenMode[WES_MULTITEX_NUM];
    umat4_t		uTexGenMat[WES_MULTITEX_NUM];

    uvec4_t     uClipPlane[WES_CLIPPLANE_NUM];

    umat4_t     uMVP;
    umat4_t     uMV;
    umat4_t     uMVIT;

    uint_t      uAlphaFunc;
    ufloat_t    uAlphaRef;
};

//variables
extern uniforms_t   u_state;
extern GLenum       u_activetex;

extern GLvoid       wes_state_init();
extern GLvoid       wes_state_update();

#endif
