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
#include "wes_shader.h"
#include "wes_matrix.h"



#define LocateUniform(A)                            \
    u_state.A.loc = wes_gl->glGetUniformLocation(sh_program, #A)

#define UpdateUniform1i(A)                                          \
    if (u_state.A.mod) {                                         \
        u_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform1i(u_state.A.loc, u_state.A.i);      \
    };
#define UpdateUniform2i(A)                                          \
    if (u_state.A.mod) {                                         \
        u_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform2iv(u_state.A.loc, 1, u_state.A.v);      \
    };
#define UpdateUniform3i(A)                                          \
    if (u_state.A.mod) {                                         \
        u_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform3iv(u_state.A.loc, 1, u_state.A.v);      \
    };
#define UpdateUniform4i(A)                                          \
    if (u_state.A.mod) {                                         \
        u_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform4iv(u_state.A.loc, 1, u_state.A.v);      \
    };
#define UpdateUniform1f(A)                                          \
    if (u_state.A.mod) {                                         \
        u_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform1f(u_state.A.loc, u_state.A.f);      \
    };
#define UpdateUniform2f(A)                                          \
    if (u_state.A.mod) {                                         \
        u_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform2fv(u_state.A.loc, 1, u_state.A.v);  \
    };
#define UpdateUniform3f(A)                                          \
    if (u_state.A.mod) {                                         \
        u_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform3fv(u_state.A.loc, 1, u_state.A.v);  \
    };
#define UpdateUniform4f(A)                                          \
    if (u_state.A.mod) {                                         \
        u_state.A.mod = GL_FALSE;                                \
        wes_gl->glUniform4fv(u_state.A.loc, 1, u_state.A.v);  \
    };

#define SetUniform1i(A, P)                                          \
    u_state.A.mod = GL_TRUE; u_state.A.i = P

#define SetUniform1f(A, P)                                          \
    u_state.A.mod = GL_TRUE; u_state.A.f = P
#define SetUniform2f(A, P0, P1)                                     \
    u_state.A.mod = GL_TRUE;                                     \
    u_state.A.v[0] = P0;                                         \
    u_state.A.v[1] = P1;
#define SetUniform3f(A, P0, P1, P2)                                 \
    u_state.A.mod = GL_TRUE;                                     \
    u_state.A.v[0] = P0;                                         \
    u_state.A.v[1] = P1;                                         \
    u_state.A.v[2] = P2;
#define SetUniform4f(A, P0, P1, P2, P3)                             \
    u_state.A.mod = GL_TRUE;                                     \
    u_state.A.v[0] = P0;                                         \
    u_state.A.v[1] = P1;                                         \
    u_state.A.v[2] = P2;                                         \
    u_state.A.v[3] = P3;

#define SetUniform2i    SetUniform2f
#define SetUniform3i    SetUniform3f
#define SetUniform4i    SetUniform4f

#define SetUniformIndex(A, P0, P1)                              \
    u_state.A.mod = GL_TRUE;                                     \
    u_state.A.v[P0] = P1;

#define SetUniform2fv(A, P)                                         \
    u_state.A.mod = GL_TRUE;                                     \
    u_state.A.v[0] = P[0];                                       \
    u_state.A.v[1] = P[1];
#define SetUniform3fv(A, P)                                         \
    u_state.A.mod = GL_TRUE;                                     \
    u_state.A.v[0] = P[0];                                       \
    u_state.A.v[1] = P[1];                                       \
    u_state.A.v[2] = P[2];
#define SetUniform4fv(A, P)                                         \
    u_state. A .mod = GL_TRUE;                                     \
    u_state. A .v[0] = P[0];                                       \
    u_state. A .v[1] = P[1];                                       \
    u_state. A .v[2] = P[2];                                       \
    u_state. A .v[3] = P[3];

uniforms_t      u_state;
GLenum          u_activetex;

GLvoid
wes_state_update()
{
    int i;

    UpdateUniform1i(uEnableRescaleNormal);
    UpdateUniform1i(uEnableNormalize);
    for(i = 0; i != WES_MULTITEX_NUM; i++){
        UpdateUniform4i(uEnableTextureGen[i]);
    }
    for(i = 0; i != WES_CLIPPLANE_NUM; i++){
        UpdateUniform1i(uEnableClipPlane[i]);
    }
    UpdateUniform1i(uEnableColorMaterial);
    UpdateUniform1i(uEnableFog);
    UpdateUniform1i(uEnableAlphaTest);
    UpdateUniform1i(uEnableFogCoord);
    UpdateUniform1i(uEnableLighting);

    for(i = 0; i != WES_LIGHT_NUM; i++){
        UpdateUniform1i(uEnableLight[i]);
        UpdateUniform4f(uLight[i].Position);
        UpdateUniform3f(uLight[i].Attenuation);
        UpdateUniform4f(uLight[i].ColorAmbient);
        UpdateUniform4f(uLight[i].ColorDiffuse);
        UpdateUniform4f(uLight[i].ColorSpec);
        UpdateUniform3f(uLight[i].SpotDir);
        UpdateUniform2f(uLight[i].SpotVar);
    }
    for(i = 0; i < WES_MULTITEX_NUM; i++){
        UpdateUniform3i(uTexture[i].Func);
        UpdateUniform4i(uTexture[i].Arg0);
        UpdateUniform4i(uTexture[i].Arg1);
        UpdateUniform4i(uTexture[i].Arg2);
        UpdateUniform4f(uTexture[i].EnvColor);
    }
    UpdateUniform1f(uRescaleFactor);

    for(i = 0; i != WES_FACE_NUM; i++){
        UpdateUniform1i(uMaterial[i].ColorMaterial);
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

    UpdateUniform1i(uFogMode);
    UpdateUniform1f(uFogDensity);
    UpdateUniform1f(uFogStart);
    UpdateUniform1f(uFogEnd);
    UpdateUniform4f(uFogColor);
    UpdateUniform1i(uAlphaFunc);
    UpdateUniform1f(uAlphaRef);

    if (m_modelview_mod){
        wes_gl->glUniformMatrix4fv(u_state.uMV.loc, 1, GL_FALSE, m_modelview->data);
    }
    if (wes_matrix_mvp()){
        wes_gl->glUniformMatrix4fv(u_state.uMVP.loc, 1, GL_FALSE, m_modelview_proj->data);
    }
    if (wes_matrix_normal()){
        wes_gl->glUniformMatrix3fv(u_state.uMVIT.loc, 1, GL_FALSE, m_modelview_normal->data);
    }
    wes_matrix_update();
}

GLvoid
wes_state_loc()
{
    int i;
    char str[32];

    LocateUniform(uEnableRescaleNormal);
    LocateUniform(uEnableNormalize);
    for(i = 0; i != WES_MULTITEX_NUM; i++){
        sprintf(str, "uEnableTextureGen[%i]", i);
        u_state.uEnableTextureGen[i].loc      = wes_gl->glGetUniformLocation(sh_program, str);
    }

    for(i = 0; i != WES_CLIPPLANE_NUM; i++){
        sprintf(str, "uEnableClipPlane[%i]", i);
        u_state.uEnableClipPlane[i].loc = wes_gl->glGetUniformLocation(sh_program, str);
    }


    LocateUniform(uEnableFog);
    LocateUniform(uEnableAlphaTest);
    LocateUniform(uEnableFogCoord);

    u_state.uEnableLighting.loc         = wes_gl->glGetUniformLocation(sh_program, "uEnableLighting");
    for(i = 0; i != WES_LIGHT_NUM; i++){
        sprintf(str, "uEnableLight[%i]", i);
        u_state.uEnableLight[i].loc     = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].Position", i);
        u_state.uLight[i].Position.loc = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].Attenuation", i);
        u_state.uLight[i].Attenuation.loc = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].ColorAmbient", i);
        u_state.uLight[i].ColorAmbient.loc  = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].ColorDiffuse", i);
        u_state.uLight[i].ColorDiffuse.loc  = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].ColorSpec", i);
        u_state.uLight[i].ColorSpec.loc  = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].SpotDir", i);
        u_state.uLight[i].SpotDir.loc  = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uLight[%i].SpotVar", i);
        u_state.uLight[i].SpotVar.loc  = wes_gl->glGetUniformLocation(sh_program, str);
    }

    LocateUniform(uLightModel.ColorAmbient);
    LocateUniform(uLightModel.TwoSided);
    LocateUniform(uLightModel.LocalViewer);
    LocateUniform(uLightModel.ColorControl);

    u_state.uRescaleFactor.loc  = wes_gl->glGetUniformLocation(sh_program, "uRescaleFactor");

    u_state.uMaterial[0].ColorAmbient.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].ColorAmbient");
    u_state.uMaterial[0].ColorDiffuse.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].ColorDiffuse");
    u_state.uMaterial[0].ColorSpec.loc     = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].ColorSpec");
    u_state.uMaterial[0].ColorEmissive.loc = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].ColorEmissive");
    u_state.uMaterial[0].SpecExponent.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].SpecExponent");
    u_state.uMaterial[0].ColorMaterial.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[0].ColorMaterial");

    u_state.uMaterial[1].ColorAmbient.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].ColorAmbient");
    u_state.uMaterial[1].ColorDiffuse.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].ColorDiffuse");
    u_state.uMaterial[1].ColorSpec.loc     = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].ColorSpec");
    u_state.uMaterial[1].ColorEmissive.loc = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].ColorEmissive");
    u_state.uMaterial[1].SpecExponent.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].SpecExponent");
    u_state.uMaterial[1].ColorMaterial.loc  = wes_gl->glGetUniformLocation(sh_program, "uMaterial[1].ColorMaterial");

    u_state.uFogMode.loc    = wes_gl->glGetUniformLocation(sh_program, "uFogMode");
    u_state.uFogDensity.loc = wes_gl->glGetUniformLocation(sh_program, "uFogDensity");
    u_state.uFogStart.loc   = wes_gl->glGetUniformLocation(sh_program, "uFogStart");
    u_state.uFogEnd.loc     = wes_gl->glGetUniformLocation(sh_program, "uFogEnd");
    u_state.uFogColor.loc   = wes_gl->glGetUniformLocation(sh_program, "uFogColor");

    for(i = 0; i != WES_CLIPPLANE_NUM; i++){
        sprintf(str, "uClipPlane[%i]", i);
        u_state.uClipPlane[i].loc = wes_gl->glGetUniformLocation(sh_program, str);
    }

    u_state.uMVP.loc    = wes_gl->glGetUniformLocation(sh_program, "uMVP");
    u_state.uMV.loc     = wes_gl->glGetUniformLocation(sh_program, "uMV");
    u_state.uMVIT.loc   = wes_gl->glGetUniformLocation(sh_program, "uMVIT");

    u_state.uAlphaFunc.loc  = wes_gl->glGetUniformLocation(sh_program, "uAlphaFunc");
    u_state.uAlphaRef.loc   = wes_gl->glGetUniformLocation(sh_program, "uAlphaRef");

    for(i = 0; i != WES_MULTITEX_NUM; i++){
        sprintf(str, "uTexture[%i].Unit", i);
        u_state.uTexture[i].Unit.loc   = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uTexture[%i].EnvColor", i);
        u_state.uTexture[i].EnvColor.loc   = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uTexture[%i].Func", i);
        u_state.uTexture[i].Func.loc   = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uTexture[%i].Arg0", i);
        u_state.uTexture[i].Arg0.loc   = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uTexture[%i].Arg1", i);
        u_state.uTexture[i].Arg1.loc   = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uTexture[%i].Arg2", i);
        u_state.uTexture[i].Arg2.loc   = wes_gl->glGetUniformLocation(sh_program, str);
    }
}

GLvoid wes_state_init()
{

    wes_gl->glUseProgram(sh_program);
    wes_state_loc();
    u_activetex = 0;

    int i;

    SetUniform1i(uEnableRescaleNormal, 0);
    SetUniform1i(uEnableNormalize, 0);
    for(i = 0; i != WES_CLIPPLANE_NUM; i++){
        SetUniform4i(uEnableTextureGen[i], 0, 0, 0, 0);
    }
    for(i = 0; i != WES_CLIPPLANE_NUM; i++){
        SetUniform1i(uEnableClipPlane[i], 0);
    }
    SetUniform1i(uEnableLighting, 0);
    SetUniform1i(uEnableColorMaterial, 0);
    SetUniform1i(uEnableFog, 0);
    SetUniform1i(uEnableAlphaTest, 0);
    SetUniform1i(uEnableFogCoord, 0);

    for(i = 0; i != WES_LIGHT_NUM; i++){
        SetUniform1i(uEnableLight[i], 0);
        SetUniform4f(uLight[i].Position, 0.0, 0.0, 1.0, 0.0);
        SetUniform3f(uLight[i].Attenuation, 1.0, 0.0, 0.0);
        SetUniform4f(uLight[i].ColorAmbient, 0.0, 0.0, 0.0, 1.0);
        SetUniform4f(uLight[i].ColorDiffuse, 0.0, 0.0, 0.0, 1.0);
        SetUniform4f(uLight[i].ColorSpec, 0.0, 0.0, 0.0, 1.0);
        SetUniform3f(uLight[i].SpotDir, 0.0, 0.0, -1.0);
        SetUniform2f(uLight[i].SpotVar, 0.0, 180.0);
    }
    /* GL_LIGHT0's default parameters differ in these cases: */
    SetUniform4f(uLight[0].ColorDiffuse, 1.0, 1.0, 1.0, 1.0);
    SetUniform4f(uLight[0].ColorSpec, 1.0, 1.0, 1.0, 1.0);

    SetUniform1f(uRescaleFactor, 1.0);

    for(i = 0; i != WES_FACE_NUM; i++){
        SetUniform4f(uMaterial[i].ColorAmbient, 0.2, 0.2, 0.2, 1.0);
        SetUniform4f(uMaterial[i].ColorDiffuse, 0.8, 0.8, 0.8, 1.0);
        SetUniform4f(uMaterial[i].ColorSpec, 0.0, 0.0, 0.0, 1.0);
        SetUniform4f(uMaterial[i].ColorEmissive, 0.0, 0.0, 0.0, 1.0);
        SetUniform1f(uMaterial[i].SpecExponent, 0.0);
        SetUniform1i(uMaterial[i].ColorMaterial, 0);
    }

    SetUniform4f(uLightModel.ColorAmbient, 0.0, 0.0, 0.0, 1.0);
    SetUniform1i(uLightModel.TwoSided, GL_FALSE);
    SetUniform1i(uLightModel.LocalViewer, GL_FALSE);
    SetUniform1i(uLightModel.ColorControl, 0);

    SetUniform1i(uFogMode, 1);
    SetUniform1f(uFogDensity, 1.0);
    SetUniform1f(uFogStart, 0.0);
    SetUniform1f(uFogEnd, 1.0);
    SetUniform4f(uFogColor, 0.0, 0.0, 0.0, 0.0);


    for(i = 0; i < WES_MULTITEX_NUM; i++){
        SetUniform1i(uTexture[i].Unit, i);
        SetUniform3i(uTexture[i].Func, 0, 0, 0);
        SetUniform4i(uTexture[i].Arg0, 2, 2, 2, 2);
        SetUniform4i(uTexture[i].Arg1, 2, 2, 2, 2);
        SetUniform4i(uTexture[i].Arg2, 2, 2, 2, 2);
        SetUniform4f(uTexture[i].EnvColor, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    SetUniform3i(uTexture[0].Func, 2, 2, 2);

    SetUniform1i(uAlphaFunc, 0);
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
        case GL_TEXTURE_GEN_S:
            SetUniformIndex(uEnableTextureGen[u_activetex], 0, b);   break;
        case GL_TEXTURE_GEN_T:
            SetUniformIndex(uEnableTextureGen[u_activetex], 1, b);   break;
        case GL_TEXTURE_GEN_R:
            SetUniformIndex(uEnableTextureGen[u_activetex], 2, b);   break;
        case GL_TEXTURE_GEN_Q:
            SetUniformIndex(uEnableTextureGen[u_activetex], 3, b);      break;
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
            SetUniformIndex(uLight[ind].SpotVar, 0, params);   break;
        case GL_SPOT_CUTOFF:
            SetUniformIndex(uLight[ind].SpotVar, 1, params);   break;
        case GL_CONSTANT_ATTENUATION:
            SetUniformIndex(uLight[ind].Attenuation, 0, params);   break;
        case GL_LINEAR_ATTENUATION:
            SetUniformIndex(uLight[ind].Attenuation, 1, params);   break;
        case GL_QUADRATIC_ATTENUATION:
            SetUniformIndex(uLight[ind].Attenuation, 2, params);   break;
        default:
            PRINT_ERROR("Invalid glLightf parameter name\n");
    }
}

GLvoid
glLightfv(GLenum light, GLenum pname, GLfloat *params)
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
            SetUniform4fv(uLight[ind].Position, tmp);
            break;
        }

        case GL_SPOT_DIRECTION:{
            GLfloat tmp[4];
            wes_matvec4(m_modelview, params, tmp);
            SetUniform3fv(uLight[ind].SpotDir, tmp);
            break;
            }

        case GL_SPOT_EXPONENT:
            SetUniformIndex(uLight[ind].SpotVar, 0, *params);   break;
        case GL_SPOT_CUTOFF:
            SetUniformIndex(uLight[ind].SpotVar, 1, *params);   break;
        case GL_CONSTANT_ATTENUATION:
            SetUniformIndex(uLight[ind].Attenuation, 0, *params);   break;
        case GL_LINEAR_ATTENUATION:
            SetUniformIndex(uLight[ind].Attenuation, 1, *params);   break;
        case GL_QUADRATIC_ATTENUATION:
            SetUniformIndex(uLight[ind].Attenuation, 2, *params);   break;

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
glMaterialfv(GLenum face, GLenum pname, GLfloat *params)
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
    if (func == GL_NEVER){
        SetUniform1i(uAlphaFunc, 0);
    } else if (func == GL_LESS){
        SetUniform1i(uAlphaFunc, 1);
    } else if (func == GL_EQUAL){
        SetUniform1i(uAlphaFunc, 2);
    } else if (func == GL_LEQUAL){
        SetUniform1i(uAlphaFunc, 3);
    } else if (func == GL_GREATER){
        SetUniform1i(uAlphaFunc, 4);
    } else if (func == GL_NOTEQUAL){
        SetUniform1i(uAlphaFunc, 5);
    } else if (func == GL_GEQUAL){
        SetUniform1i(uAlphaFunc, 6);
    } else if (func == GL_ALWAYS){
        SetUniform1i(uAlphaFunc, 7);
    }
    SetUniform1f(uAlphaRef, ref);
}

GLvoid
glFogi(GLenum pname, GLint param)
{
    switch(pname)
    {
        case GL_FOG_MODE:
            if (param == GL_LINEAR){
                SetUniform1i(uFogMode, 0);
            } else if (param == GL_EXP){
                SetUniform1i(uFogMode, 1);
            } else if (param == GL_EXP2){
                SetUniform1i(uFogMode, 2);
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

GLvoid
glTexGeni(GLenum coord, GLenum pname, GLint param)
{
    switch(pname)
    {
        case GL_TEXTURE_GEN_MODE:
            SetUniformIndex(uTexGenMode[u_activetex], coord - GL_S, param);
            break;
    }
}

GLvoid
glTexGenfv(GLenum coord, GLenum pname, GLfloat* param)
{
    switch(pname)
    {
        case GL_OBJECT_PLANE: break;
        case GL_EYE_PLANE: break;
    }

}

GLvoid
glActiveTexture(GLenum texture)
{
    u_activetex = texture;
    wes_gl->glActiveTexture(texture);
}

GLint
wes_index_envfunc(GLint param)
{
    switch(param)
    {
        case GL_REPLACE:        return 1;
        case GL_MODULATE:       return 2;
        case GL_ADD:            return 3;
        case GL_ADD_SIGNED:     return 4;
        case GL_INTERPOLATE:    return 5;
        case GL_SUBTRACT:       return 6;
        case GL_DOT3_RGB:       return 7;
        case GL_DOT3_RGBA:      return 8;
        case GL_DECAL:          return 9;
        case GL_BLEND:          return 10;
        case GL_COMBINE:        return 11;
        default:                return 0;
    }

}

GLint
wes_index_envsrc(GLint param)
{
    switch(param)
    {
        case GL_PREVIOUS:       return 1;
        case GL_CONSTANT:       return 2;
        case GL_PRIMARY_COLOR:  return 3;
        case GL_TEXTURE:        return 4;
        default:                return 0;
    }

}

GLint
wes_index_envop(GLint param)
{
    switch(param)
    {
        case GL_SRC_ALPHA:              return 1;
        case GL_ONE_MINUS_SRC_ALPHA:    return 2;
        case GL_SRC_COLOR:              return 3;
        case GL_ONE_MINUS_SRC_COLOR:    return 4;
        default:                        return 0;
    }

}

GLvoid
glTexEnvi(GLenum target, GLenum pname, GLint param)
{
    if (target == GL_TEXTURE_ENV){
        switch(pname)
        {
            case GL_TEXTURE_ENV_MODE:
                SetUniformIndex(uTexture[u_activetex].Func, 0, wes_index_envfunc(param));
                break;

            case GL_COMBINE_RGB:
                SetUniformIndex(uTexture[u_activetex].Func, 1, wes_index_envfunc(param));
                break;

            case GL_COMBINE_ALPHA:
                SetUniformIndex(uTexture[u_activetex].Func, 2, wes_index_envfunc(param));
                break;

            case GL_SOURCE0_RGB:
                SetUniformIndex(uTexture[u_activetex].Arg0, 0, wes_index_envsrc(param));
                break;

            case GL_OPERAND0_RGB:
                SetUniformIndex(uTexture[u_activetex].Arg0, 1, wes_index_envop(param));
                break;

            case GL_SOURCE0_ALPHA:
                SetUniformIndex(uTexture[u_activetex].Arg0, 2, wes_index_envsrc(param));
                break;

            case GL_OPERAND0_ALPHA:
                SetUniformIndex(uTexture[u_activetex].Arg0, 3, wes_index_envop(param));
                break;

            case GL_SOURCE1_RGB:
                SetUniformIndex(uTexture[u_activetex].Arg1, 0, wes_index_envsrc(param));
                break;

            case GL_OPERAND1_RGB:
                SetUniformIndex(uTexture[u_activetex].Arg1, 1, wes_index_envop(param));
                break;

            case GL_SOURCE1_ALPHA:
                SetUniformIndex(uTexture[u_activetex].Arg1, 2, wes_index_envsrc(param));
                break;

            case GL_OPERAND1_ALPHA:
                SetUniformIndex(uTexture[u_activetex].Arg1, 3, wes_index_envop(param));
                break;

            case GL_SOURCE2_RGB:
                SetUniformIndex(uTexture[u_activetex].Arg2, 0, wes_index_envsrc(param));
                break;

            case GL_OPERAND2_RGB:
                SetUniformIndex(uTexture[u_activetex].Arg2, 1, wes_index_envop(param));
                break;

            case GL_SOURCE2_ALPHA:
                SetUniformIndex(uTexture[u_activetex].Arg2, 2, wes_index_envsrc(param));
                break;

            case GL_OPERAND2_ALPHA:
                SetUniformIndex(uTexture[u_activetex].Arg2, 3, wes_index_envop(param));
                break;

        }
    }
}


GLvoid
glTexEnvfv(GLenum target, GLenum pname, GLfloat *param)
{
    if (target == GL_TEXTURE_ENV){
        switch(pname)
        {
            case GL_TEXTURE_ENV_COLOR:
                SetUniform4fv(uTexture[u_activetex].EnvColor, param);
                break;
        }
    }
}


