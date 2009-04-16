

#ifndef __WES_UNIFORM_H__
#define __WES_UNIFORM_H__

typedef struct uvec4_s      uvec4_t;
typedef struct uvec3_s      uvec3_t;
typedef struct uvec2_s      uvec2_t;
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
    ubool_t uEnableRescaleNormal;
    ubool_t uEnableNormalize;
    ubool_t uEnableTextureGenS;
    ubool_t uEnableTextureGenT;
    ubool_t uEnableTextureGenR;
    ubool_t uEnableTextureGenQ;
    ubool_t uEnableClipPlane[6];
    ubool_t uEnableLighting;
    ubool_t uEnableLight[8];
    ubool_t uEnableColorMaterial;
    ubool_t uEnableFog;
    ubool_t uEnableAlphaTest;
    ubool_t uEnableFogCoord;

    usampler2d_t uTexture0;

    struct {
        uvec4_t     Position, ColorAmbient, ColorDiffuse, ColorSpec;
        uvec3_t     Attenuation, SpotDir;
        ufloat_t    SpotExponent, SpotCutoff;
    } uLight[8];

    struct {
        uvec4_t     ColorAmbient, ColorDiffuse, ColorSpec, ColorEmissive;
        ufloat_t 	SpecExponent;
    } uMaterial[2];

    struct {
    	uvec4_t     ColorAmbient;
        ubool_t     TwoSided, LocalViewer;
        uint_t      ColorControl;
    } uLightModel;

    ufloat_t    uRescaleFactor;

    uint_t      uFogType;
    ufloat_t    uFogStart, uFogEnd, uFogDensity;
    uvec4_t     uFogColor;

    uvec4_t     uClipPlane[6];

    umat4_t     uMVP;
    umat4_t     uMV;
    umat4_t     uMVIT;

    uint_t      uAlphaFunc;
    ufloat_t    uAlphaRef;
};

//variables
extern uniforms_t   uf_state;

extern GLvoid       wes_state_init();
extern GLvoid       wes_state_update();

#endif
