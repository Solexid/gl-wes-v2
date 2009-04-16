#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wes.h"
#include "wes_uniform.h"
#include "wes_shader.h"
#include "wes_matrix.h"

#define UpdateUniform1i(A)                                          \
    if (uf_state.A.mod) {                                         \
        uf_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform1i(uf_state.A.loc, uf_state.A.i);      \
    };
#define UpdateUniform1f(A)                                          \
    if (uf_state.A.mod) {                                         \
        uf_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform1f(uf_state.A.loc, uf_state.A.f);      \
    };
#define UpdateUniform2f(A)                                          \
    if (uf_state.A.mod) {                                         \
        uf_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform2fv(uf_state.A.loc, 1, uf_state.A.v);  \
    };
#define UpdateUniform3f(A)                                          \
    if (uf_state.A.mod) {                                         \
        uf_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform3fv(uf_state.A.loc, 1, uf_state.A.v);  \
    };
#define UpdateUniform4f(A)                                          \
    if (uf_state.A.mod) {                                         \
        uf_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform4fv(uf_state.A.loc, 1, uf_state.A.v);  \
    };

#define SetUniform1i(A, P)                                          \
    uf_state.A.mod = GL_TRUE; uf_state.A.i = P
#define SetUniform1f(A, P)                                          \
    uf_state.A.mod = GL_TRUE; uf_state.A.f = P
#define SetUniform2f(A, P0, P1)                                     \
    uf_state.A.mod = GL_TRUE;                                     \
    uf_state.A.v[0] = P0;                                         \
    uf_state.A.v[1] = P1;
#define SetUniform3f(A, P0, P1, P2)                                 \
    uf_state.A.mod = GL_TRUE;                                     \
    uf_state.A.v[0] = P0;                                         \
    uf_state.A.v[1] = P1;                                         \
    uf_state.A.v[2] = P2;
#define SetUniform4f(A, P0, P1, P2, P3)                             \
    uf_state.A.mod = GL_TRUE;                                     \
    uf_state.A.v[0] = P0;                                         \
    uf_state.A.v[1] = P1;                                         \
    uf_state.A.v[2] = P2;                                         \
    uf_state.A.v[3] = P3;

#define SetUniform2fv(A, P)                                         \
    uf_state.A.mod = GL_TRUE;                                     \
    uf_state.A.v[0] = P[0];                                       \
    uf_state.A.v[1] = P[1];
#define SetUniform3fv(A, P)                                         \
    uf_state.A.mod = GL_TRUE;                                     \
    uf_state.A.v[0] = P[0];                                       \
    uf_state.A.v[1] = P[1];                                       \
    uf_state.A.v[2] = P[2];
#define SetUniform4fv(A, P)                                         \
    uf_state.A.mod = GL_TRUE;                                     \
    uf_state.A.v[0] = P[0];                                       \
    uf_state.A.v[1] = P[1];                                       \
    uf_state.A.v[2] = P[2];                                       \
    uf_state.A.v[3] = P[3];

uniforms_t      uf_state;


GLvoid
wes_state_update()
{
    int i;

    UpdateUniform1i(uEnableRescaleNormal);
    UpdateUniform1i(uEnableNormalize);
    UpdateUniform1i(uEnableTextureGenS);
    UpdateUniform1i(uEnableTextureGenT);
    UpdateUniform1i(uEnableTextureGenR);
    UpdateUniform1i(uEnableTextureGenQ);
    for(i = 0; i != 6; i++){
        UpdateUniform1i(uEnableClipPlane[i]);
    }
    UpdateUniform1i(uEnableLighting);
    for(i = 0; i != 8; i++){
        UpdateUniform1i(uEnableLight[i]);
    }
    UpdateUniform1i(uEnableColorMaterial);
    UpdateUniform1i(uEnableFog);
    UpdateUniform1i(uEnableAlphaTest);
    UpdateUniform1i(uEnableFogCoord);

    for(i = 0; i != 8; i++){
        UpdateUniform4f(uLight[i].Position);
        UpdateUniform3f(uLight[i].Attenuation);
        UpdateUniform4f(uLight[i].ColorAmbient);
        UpdateUniform4f(uLight[i].ColorDiffuse);
        UpdateUniform4f(uLight[i].ColorSpec);
        UpdateUniform3f(uLight[i].SpotDir);
        UpdateUniform1f(uLight[i].SpotExponent);
        UpdateUniform1f(uLight[i].SpotCutoff);
    }

    UpdateUniform1f(uRescaleFactor);

    for(i = 0; i != 2; i++){
        UpdateUniform4f(uMaterial[i].ColorAmbient);
        UpdateUniform4f(uMaterial[i].ColorDiffuse);
        UpdateUniform4f(uMaterial[i].ColorSpec);
        UpdateUniform4f(uMaterial[i].ColorEmissive);
        UpdateUniform1f(uMaterial[i].SpecExponent);
    }
    UpdateUniform4f(uLightModel.ColorAmbient);
    UpdateUniform1i(uLightModel.TwoSided);
    UpdateUniform1i(uLightModel.LocalViewer);
    UpdateUniform1i(uLightModel.ColorControl);

    UpdateUniform1i(uFogType);
    UpdateUniform1f(uFogDensity);
    UpdateUniform1f(uFogStart);
    UpdateUniform1f(uFogEnd);
    UpdateUniform4f(uFogColor);
    UpdateUniform1i(uTexture0);
    UpdateUniform1i(uAlphaFunc);
    UpdateUniform1f(uAlphaRef);

    if (m_modelview_mod){
        wes_gl->glUniformMatrix4fv(uf_state.uMV.loc, 1, GL_FALSE, m_modelview->data);
    }
    if (wes_matrix_mvp()){
        wes_gl->glUniformMatrix4fv(uf_state.uMVP.loc, 1, GL_FALSE, m_modelview_proj->data);
    }
    if (wes_matrix_mvit()){
        wes_gl->glUniformMatrix3fv(uf_state.uMVIT.loc, 1, GL_FALSE, m_modelview_it3);
    }
    wes_matrix_update();
}

GLvoid
wes_state_loc()
{
    int i;
    char str[32];

    uf_state.uEnableRescaleNormal.loc    = wes_gl->glGetUniformLocation(sh_program, "uEnableRescaleNormal" );
    uf_state.uEnableNormalize.loc        = wes_gl->glGetUniformLocation(sh_program, "uEnableNormalize" );
    uf_state.uEnableTextureGenS.loc      = wes_gl->glGetUniformLocation(sh_program, "uEnableTextureGenS");
    uf_state.uEnableTextureGenT.loc      = wes_gl->glGetUniformLocation(sh_program, "uEnableTextureGenT");
    uf_state.uEnableTextureGenR.loc      = wes_gl->glGetUniformLocation(sh_program, "uEnableTextureGenR");
    uf_state.uEnableTextureGenQ.loc      = wes_gl->glGetUniformLocation(sh_program, "uEnableTextureGenQ");
    for(i = 0; i != 6; i++){
        sprintf(str, "uEnableClipPlane[%i]", i);
        uf_state.uEnableClipPlane[i].loc = wes_gl->glGetUniformLocation(sh_program, str);
    }
    uf_state.uEnableLighting.loc         = wes_gl->glGetUniformLocation(sh_program, "uEnableLighting");
    for(i = 0; i != 8; i++){
        sprintf(str, "uEnableLight[%i]", i);
        uf_state.uEnableLight[i].loc     = wes_gl->glGetUniformLocation(sh_program, str);
    }
    uf_state.uEnableColorMaterial.loc    = wes_gl->glGetUniformLocation(sh_program, "uEnableColorMaterial" );
    uf_state.uEnableFog.loc              = wes_gl->glGetUniformLocation(sh_program, "uEnableFog");
    uf_state.uEnableAlphaTest.loc        = wes_gl->glGetUniformLocation(sh_program, "uEnableAlphaTest");
    uf_state.uEnableFogCoord.loc         = wes_gl->glGetUniformLocation(sh_program, "uEnableFogCoord");

    for(i = 0; i != 8; i++){
        sprintf(str, "uLight[%i].Position", i);
        uf_state.uLight[i].Position.loc = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].Attenuation", i);
        uf_state.uLight[i].Attenuation.loc = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].ColorAmbient", i);
        uf_state.uLight[i].ColorAmbient.loc  = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].ColorDiffuse", i);
        uf_state.uLight[i].ColorDiffuse.loc  = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].ColorSpec", i);
        uf_state.uLight[i].ColorSpec.loc  = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].SpotDir", i);
        uf_state.uLight[i].SpotDir.loc  = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].SpotExponent", i);
        uf_state.uLight[i].SpotExponent.loc  = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].SpotCutoff", i);
        uf_state.uLight[i].SpotCutoff.loc  = wes_gl->glGetUniformLocation(sh_program, str);
    }

    uf_state.uRescaleFactor.loc  = wes_gl->glGetUniformLocation(sh_program, "uRescaleFactor");

    uf_state.uMaterial[0].ColorAmbient.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].ColorAmbient");
    uf_state.uMaterial[0].ColorDiffuse.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].ColorDiffuse");
    uf_state.uMaterial[0].ColorSpec.loc     = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].ColorSpec");
    uf_state.uMaterial[0].ColorEmissive.loc = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].ColorEmissive");
    uf_state.uMaterial[0].SpecExponent.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].SpecExponent");
    uf_state.uMaterial[1].ColorAmbient.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].ColorAmbient");
    uf_state.uMaterial[1].ColorDiffuse.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].ColorDiffuse");
    uf_state.uMaterial[1].ColorSpec.loc     = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].ColorSpec");
    uf_state.uMaterial[1].ColorEmissive.loc = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].ColorEmissive");
    uf_state.uMaterial[1].SpecExponent.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].SpecExponent");

    uf_state.uFogType.loc    = wes_gl->glGetUniformLocation(sh_program, "uFogType");
    uf_state.uFogDensity.loc = wes_gl->glGetUniformLocation(sh_program, "uFogDensity");
    uf_state.uFogStart.loc   = wes_gl->glGetUniformLocation(sh_program, "uFogStart");
    uf_state.uFogEnd.loc     = wes_gl->glGetUniformLocation(sh_program, "uFogEnd");
    uf_state.uFogColor.loc   = wes_gl->glGetUniformLocation(sh_program, "uFogColor");

    for(i = 0; i != 6; i++){
        sprintf(str, "uClipPlane[%i]", i);
        uf_state.uClipPlane[i].loc = wes_gl->glGetUniformLocation(sh_program, str);
    }

    uf_state.uMVP.loc    = wes_gl->glGetUniformLocation(sh_program, "uMVP");
    uf_state.uMV.loc     = wes_gl->glGetUniformLocation(sh_program, "uMV");
    uf_state.uMVIT.loc   = wes_gl->glGetUniformLocation(sh_program, "uMVIT");

    uf_state.uAlphaFunc.loc  = wes_gl->glGetUniformLocation(sh_program, "uAlphaFunc");
    uf_state.uAlphaRef.loc   = wes_gl->glGetUniformLocation(sh_program, "uAlphaRef");
    uf_state.uTexture0.loc   = wes_gl->glGetUniformLocation(sh_program, "uTexture0");
}

GLvoid wes_state_init()
{

    wes_gl->glUseProgram(sh_program);
    wes_state_loc();

    int i;

    SetUniform1i(uEnableRescaleNormal, 0);
    SetUniform1i(uEnableNormalize, 0);
    SetUniform1i(uEnableTextureGenS, 0);
    SetUniform1i(uEnableTextureGenT, 0);
    SetUniform1i(uEnableTextureGenR, 0);
    SetUniform1i(uEnableTextureGenQ, 0);
    for(i = 0; i != 6; i++){
        SetUniform1i(uEnableClipPlane[i], 0);
    }
    SetUniform1i(uEnableLighting, 0);
    for(i = 0; i != 8; i++){
        SetUniform1i(uEnableLight[i], 0);
    }
    SetUniform1i(uEnableColorMaterial, 0);
    SetUniform1i(uEnableFog, 0);
    SetUniform1i(uEnableAlphaTest, 0);
    SetUniform1i(uEnableFogCoord, 0);

    SetUniform1i(uTexture0, 0);

    for(i = 0; i != 8; i++){
        SetUniform4f(uLight[i].Position, 0.0, 0.0, 1.0, 0.0);
        SetUniform3f(uLight[i].Attenuation, 1.0, 0.0, 0.0);
        SetUniform4f(uLight[i].ColorAmbient, 0.0, 0.0, 0.0, 1.0);
        SetUniform4f(uLight[i].ColorDiffuse, 0.0, 0.0, 0.0, 1.0);
        SetUniform4f(uLight[i].ColorSpec, 0.0, 0.0, 0.0, 1.0);
        SetUniform3f(uLight[i].SpotDir, 0.0, 0.0, -1.0);
        SetUniform1f(uLight[i].SpotExponent, 0.0);
        SetUniform1f(uLight[i].SpotCutoff, 180.0);
    }
    /* GL_LIGHT0's default parameters differ in these cases: */
    SetUniform4f(uLight[0].ColorDiffuse, 1.0, 1.0, 1.0, 1.0);
    SetUniform4f(uLight[0].ColorSpec, 1.0, 1.0, 1.0, 1.0);

    SetUniform1f(uRescaleFactor, 1.0);

    for(i = 0; i != 2; i++){
        SetUniform4f(uMaterial[i].ColorAmbient, 0.2, 0.2, 0.2, 1.0);
        SetUniform4f(uMaterial[i].ColorDiffuse, 0.8, 0.8, 0.8, 1.0);
        SetUniform4f(uMaterial[i].ColorSpec, 0.0, 0.0, 0.0, 1.0);
        SetUniform4f(uMaterial[i].ColorEmissive, 0.0, 0.0, 0.0, 1.0);
        SetUniform1f(uMaterial[i].SpecExponent, 0.0);
    }

    SetUniform4f(uLightModel.ColorAmbient, 0.0, 0.0, 0.0, 1.0);
    SetUniform1i(uLightModel.TwoSided, GL_FALSE);
    SetUniform1i(uLightModel.LocalViewer, GL_FALSE);
    SetUniform1i(uLightModel.ColorControl, 0);

    SetUniform1i(uFogType, 1);
    SetUniform1f(uFogDensity, 1.0);
    SetUniform1f(uFogStart, 0.0);
    SetUniform1f(uFogEnd, 1.0);
    SetUniform4f(uFogColor, 0.0, 0.0, 0.0, 0.0);

    SetUniform1i(uTexture0, 0);
    SetUniform1i(uAlphaFunc, GL_ALWAYS);
    SetUniform1f(uAlphaRef, 0.0);

    wes_state_update();
}

GLvoid
wes_setstate(GLenum e, GLboolean b)
{
    switch(e)
    {
        case GL_RESCALE_NORMAL:     SetUniform1i(uEnableRescaleNormal, b);   break;
        case GL_NORMALIZE:          SetUniform1i(uEnableNormalize, b);       break;
        case GL_TEXTURE_GEN_S:      SetUniform1i(uEnableTextureGenS, b);     break;
        case GL_TEXTURE_GEN_T:      SetUniform1i(uEnableTextureGenT, b);     break;
        case GL_TEXTURE_GEN_R:      SetUniform1i(uEnableTextureGenR, b);     break;
        case GL_TEXTURE_GEN_Q:      SetUniform1i(uEnableTextureGenQ, b);     break;

        case GL_CLIP_PLANE0:        SetUniform1i(uEnableClipPlane[0], b);    break;
        case GL_CLIP_PLANE1:        SetUniform1i(uEnableClipPlane[1], b);    break;
        case GL_CLIP_PLANE2:        SetUniform1i(uEnableClipPlane[2], b);    break;
        case GL_CLIP_PLANE3:        SetUniform1i(uEnableClipPlane[3], b);    break;
        case GL_CLIP_PLANE4:        SetUniform1i(uEnableClipPlane[4], b);    break;
        case GL_CLIP_PLANE5:        SetUniform1i(uEnableClipPlane[5], b);    break;

        case GL_LIGHTING:           SetUniform1i(uEnableLighting, b);        break;
        case GL_LIGHT0:             SetUniform1i(uEnableLight[0], b);        break;
        case GL_LIGHT1:             SetUniform1i(uEnableLight[1], b);        break;
        case GL_LIGHT2:             SetUniform1i(uEnableLight[2], b);        break;
        case GL_LIGHT3:             SetUniform1i(uEnableLight[3], b);        break;
        case GL_LIGHT4:             SetUniform1i(uEnableLight[4], b);        break;
        case GL_LIGHT5:             SetUniform1i(uEnableLight[5], b);        break;
        case GL_LIGHT6:             SetUniform1i(uEnableLight[6], b);        break;
        case GL_LIGHT7:             SetUniform1i(uEnableLight[7], b);        break;
        case GL_COLOR_MATERIAL:     SetUniform1i(uEnableColorMaterial, b);   break;

        case GL_FOG:                SetUniform1i(uEnableFog, b);             break;
        case GL_ALPHA_TEST:         SetUniform1i(uEnableAlphaTest, b);       break;

        default:
            if (b){
                wes_gl->glEnable(e);
            } else {
                wes_gl->glDisable(e);
            }
            break;
    }
}

GLvoid
glLightf(GLenum light, GLenum pname, GLfloat params)
{
    GLuint ind = light - GL_LIGHT0;
    switch(pname){
        case GL_SPOT_EXPONENT:
            SetUniform1f(uLight[ind].SpotExponent, params); break;
        case GL_SPOT_CUTOFF:
            SetUniform1f(uLight[ind].SpotCutoff, params); break;
        case GL_CONSTANT_ATTENUATION:
            uf_state.uLight[ind].Attenuation.mod = GL_TRUE;
            uf_state.uLight[ind].Attenuation.v[0] = params;
            break;

        case GL_LINEAR_ATTENUATION:
            uf_state.uLight[ind].Attenuation.mod = GL_TRUE;
            uf_state.uLight[ind].Attenuation.v[1] = params;
            break;

        case GL_QUADRATIC_ATTENUATION:
            uf_state.uLight[ind].Attenuation.mod = GL_TRUE;
            uf_state.uLight[ind].Attenuation.v[2] = params;
            break;

        default:
            PRINT_ERROR("Invalid glLightf parameter name\n");
    }
}

GLvoid
glLightfv(GLenum light, GLenum pname, GLfloat* params)
{
    GLuint ind = light - GL_LIGHT0;
    switch(pname)
    {
        case GL_AMBIENT:
            SetUniform4fv(uLight[ind].ColorAmbient, params); break;
        case GL_DIFFUSE:
            SetUniform4fv(uLight[ind].ColorDiffuse, params); break;
        case GL_SPECULAR:
            SetUniform4fv(uLight[ind].ColorSpec, params); break;
        case GL_POSITION:{
            GLfloat tmp[4];
            wes_matvec4(m_modelview, params, tmp);
            SetUniform4fv(uLight[ind].Position, params);
            break;
        }
        case GL_SPOT_DIRECTION:{
            GLfloat tmp[4];
            wes_matvec4(m_modelview, params, tmp);
            SetUniform3fv(uLight[ind].SpotDir, tmp);
            break;
            }
        case GL_SPOT_EXPONENT:
            SetUniform1f(uLight[ind].SpotExponent, *params); break;
        case GL_SPOT_CUTOFF:
            SetUniform1f(uLight[ind].SpotCutoff, *params); break;

        case GL_CONSTANT_ATTENUATION:
            uf_state.uLight[ind].Attenuation.mod = GL_TRUE;
            uf_state.uLight[ind].Attenuation.v[0] = *params;
            break;

        case GL_LINEAR_ATTENUATION:
            uf_state.uLight[ind].Attenuation.mod = GL_TRUE;
            uf_state.uLight[ind].Attenuation.v[1] = *params;
            break;

        case GL_QUADRATIC_ATTENUATION:
            uf_state.uLight[ind].Attenuation.mod = GL_TRUE;
            uf_state.uLight[ind].Attenuation.v[2] = *params;
            break;

        default:
            PRINT_ERROR("Invalid glLightfv parameter name\n");
    }
}

GLvoid
glMaterialf(GLenum face, GLenum pname, GLfloat params)
{
    switch(pname)
    {
        case GL_SHININESS:
            if (face == GL_FRONT){
                SetUniform1f(uMaterial[0].SpecExponent, params);
            } else if (face == GL_BACK){
                SetUniform1f(uMaterial[1].SpecExponent, params);
            } else if (face == GL_FRONT_AND_BACK){
                SetUniform1f(uMaterial[0].SpecExponent, params);
                SetUniform1f(uMaterial[1].SpecExponent, params);
            }
            break;

        default:
            PRINT_ERROR("Invalid glMaterialf parameter name\n");
    }
}

GLvoid
glMaterialfv(GLenum face, GLenum pname, GLfloat* params)
{
    switch(pname)
    {
        case GL_AMBIENT:
            if (face == GL_FRONT){
                SetUniform4fv(uMaterial[0].ColorAmbient, params);
            } else if (face == GL_BACK){
                SetUniform4fv(uMaterial[1].ColorAmbient, params);
            } else if (face == GL_FRONT_AND_BACK){
                SetUniform4fv(uMaterial[0].ColorAmbient, params);
                SetUniform4fv(uMaterial[1].ColorAmbient, params);
            }
            break;

        case GL_DIFFUSE:
            if (face == GL_FRONT){
                SetUniform4fv(uMaterial[0].ColorDiffuse, params);
            } else if (face == GL_BACK){
                SetUniform4fv(uMaterial[1].ColorDiffuse, params);
            } else if (face == GL_FRONT_AND_BACK){
                SetUniform4fv(uMaterial[0].ColorDiffuse, params);
                SetUniform4fv(uMaterial[1].ColorDiffuse, params);
            }
            break;

        case GL_AMBIENT_AND_DIFFUSE:
            if (face == GL_FRONT){
                SetUniform4fv(uMaterial[0].ColorAmbient, params);
                SetUniform4fv(uMaterial[0].ColorDiffuse, params);
            } else if (face == GL_BACK){
                SetUniform4fv(uMaterial[1].ColorAmbient, params);
                SetUniform4fv(uMaterial[1].ColorDiffuse, params);
            } else if (face == GL_FRONT_AND_BACK){
                SetUniform4fv(uMaterial[0].ColorAmbient, params);
                SetUniform4fv(uMaterial[0].ColorDiffuse, params);
                SetUniform4fv(uMaterial[1].ColorAmbient, params);
                SetUniform4fv(uMaterial[1].ColorDiffuse, params);
            }
            break;

        case GL_SPECULAR:
            if (face == GL_FRONT){
                SetUniform4fv(uMaterial[0].ColorSpec, params);
            } else if (face == GL_BACK){
                SetUniform4fv(uMaterial[1].ColorSpec, params);
            } else if (face == GL_FRONT_AND_BACK){
                SetUniform4fv(uMaterial[0].ColorSpec, params);
                SetUniform4fv(uMaterial[1].ColorSpec, params);
            }
            break;

        case GL_EMISSION:
            if (face == GL_FRONT){
                SetUniform4fv(uMaterial[0].ColorEmissive, params);
            } else if (face == GL_BACK){
                SetUniform4fv(uMaterial[1].ColorEmissive, params);
            } else if (face == GL_FRONT_AND_BACK){
                SetUniform4fv(uMaterial[0].ColorEmissive, params);
                SetUniform4fv(uMaterial[1].ColorEmissive, params);
            }
            break;

        case GL_SHININESS:
            if (face == GL_FRONT){
                SetUniform1f(uMaterial[0].SpecExponent, *params);
            } else if (face == GL_BACK){
                SetUniform1f(uMaterial[1].SpecExponent, *params);
            } else if (face == GL_FRONT_AND_BACK){
                SetUniform1f(uMaterial[0].SpecExponent, *params);
                SetUniform1f(uMaterial[1].SpecExponent, *params);
            }
            break;

        default:
            PRINT_ERROR("Invalid glMaterialfv parameter name\n");
    }
}


GLvoid
glLightModelfv(GLenum pname, GLfloat* params)
{
    switch(pname)
    {
        case GL_LIGHT_MODEL_AMBIENT:
            SetUniform4fv(uLightModel.ColorAmbient, params); break;
    }
}

GLvoid
glLightModeli(GLenum pname, GLint params)
{
    switch(pname)
    {
        case GL_LIGHT_MODEL_TWO_SIDE:
            SetUniform1i(uLightModel.TwoSided, params); break;
        case GL_LIGHT_MODEL_LOCAL_VIEWER:
            SetUniform1i(uLightModel.LocalViewer, params); break;
        case GL_LIGHT_MODEL_COLOR_CONTROL:
            SetUniform1i(uLightModel.ColorControl, params); break;
    }
}

GLvoid
glEnable(GLenum e)
{
    wes_setstate(e, GL_TRUE);
}

GLvoid
glDisable(GLenum e)
{
    wes_setstate(e, GL_FALSE);
}

GLvoid
glAlphaFunc(GLenum func, GLclampf ref)
{
    SetUniform1i(uAlphaFunc, func);
    SetUniform1f(uAlphaRef, ref);
}

GLvoid
glFogi(GLenum pname, GLint param)
{
    switch(pname)
    {
        case GL_FOG_MODE:
            if (param == GL_LINEAR){
                SetUniform1i(uFogType, 0);
            } else if (param == GL_EXP){
                SetUniform1i(uFogType, 1);
            } else if (param == GL_EXP2){
                SetUniform1i(uFogType, 2);
            }
            break;

        case GL_FOG_COORD_SRC:
            SetUniform1i(uEnableFogCoord, (param == GL_FOG_COORD)); break;
    }
}

GLvoid
glFogf(GLenum pname, GLfloat param)
{
    switch(pname)
    {
        case GL_FOG_DENSITY:
            SetUniform1f(uFogDensity, param); break;
        case GL_FOG_START:
            SetUniform1f(uFogStart, param); break;
        case GL_FOG_END:
            SetUniform1f(uFogEnd, param); break;
    }
}

GLvoid
glFogfv(GLenum pname, GLfloat *param)
{
    switch(pname)
    {
        case GL_FOG_COLOR:
            SetUniform4fv(uFogColor, param); break;
    }
}




