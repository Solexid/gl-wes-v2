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
#include "wes_state.h"
#include "wes_shader.h"

const char* frag_header = "\
#define LIGHT_NUM						8\n\
#define CLIPPLANE_NUM					6\n\
#define MULTITEX_NUM					4\n\
#define FACE_NUM						2\n\
\n\
struct sMultiTexture {\n\
    lowp vec4 EnvColor;\n\
    sampler2D Unit;\n\
};\n\
\n\
uniform lowp vec4		uFogColor;\n\
uniform sMultiTexture	uTexture[MULTITEX_NUM];\n\
uniform	highp float		uAlphaRef;\n\
\n\
//Varyings:\n\
varying lowp vec4 		vColor;\n\
varying lowp vec2 		vFactor;\n\
varying mediump vec4 	vTexCoord[MULTITEX_NUM];\n\
\n\
void main(){\n\
    gl_FragColor = vColor;\n";


GLint
wes_frag_clipplane(char* buff, progstate_t *s)
{
    char *str = buff;

    if (s->uEnableClipPlane){
        str += sprintf(str,"%s\n", "gl_FragColor.a *= vFactor.y;");
    }

    return (GLint)(str - buff);
};

GLint
wes_frag_fog(char* buff, progstate_t *s)
{
    char *str = buff;
    if (s->uEnableFog)
    {
        str += sprintf(str,"%s\n", "gl_FragColor = gl_FragColor * vFactor.x + \
                                    (1.0 - vFactor.x) * uFogColor;");
    }
    return (GLint)(str - buff);
};

GLint
wes_frag_alphatest(char* buff, progstate_t *s)
{
    char *str = buff;
    if (s->uEnableAlphaTest){
        switch(s->uAlphaFunc){
            case WES_ALPHA_NEVER:
                str += sprintf(str, "%s\n", "discard;");
                break;

            case WES_ALPHA_LESS:
                str += sprintf(str, "%s\n", "if (gl_FragColor.w >= uAlphaRef) discard;");
                break;

            case WES_ALPHA_EQUAL:
                str += sprintf(str, "%s\n", "if (gl_FragColor.w != uAlphaRef) discard;");
                break;

            case WES_ALPHA_LEQUAL:
                str += sprintf(str, "%s\n", "if (gl_FragColor.w > uAlphaRef) discard;");
                break;

            case WES_ALPHA_GREATER:
                str += sprintf(str, "%s\n", "if (gl_FragColor.w <= uAlphaRef) discard;");
                break;

            case WES_ALPHA_NOTEQUAL:
                str += sprintf(str, "%s\n", "if (gl_FragColor.w == uAlphaRef) discard;");
                break;

            case WES_ALPHA_GEQUAL:
                str += sprintf(str, "%s\n", "if (gl_FragColor.w < uAlphaRef) discard;");
                break;

        }
    }
    return (GLint)(str - buff);
};


GLint
wes_frag_op(char *buff, progstate_t *s, int tex, int arg)
{
    char *str = buff;

    switch(s->uTexture[tex].Arg[arg].RGBOp)
    {
        case WES_OP_ALPHA:
            str += sprintf(str, "arg%i.rgb = vec3(arg%i.a, arg%i.a, arg%i.a);\n", arg, arg, arg, arg);break;
        case WES_OP_ONE_MINUS_ALPHA:
            str += sprintf(str, "arg%i.rgb = vec3(1.0 - arg%i.a, 1.0 - arg%i.a, 1.0 - arg%i.a);\n", arg, arg, arg, arg);break;
        case WES_OP_COLOR:
            break;
        case WES_OP_ONE_MINUS_COLOR:
            str += sprintf(str, "arg%i.rgb = vec3(1.0 - arg%i.r, 1.0 - arg%i.g, 1.0 - arg%i.b);\n", arg, arg, arg, arg);break;
    }

    switch(s->uTexture[tex].Arg[arg].AlphaOp)
    {
        case WES_OP_ALPHA:
            break;
        case WES_OP_ONE_MINUS_ALPHA:
            str += sprintf(str, "arg%i.a = 1.0 - arg%i.a;\n", arg, arg);break;
    }

    return (GLint)(str - buff);

}

GLint
wes_frag_arg(char *buff, progstate_t *s, int tex, int arg)
{
    char *str = buff;
    if (s->uTexture[tex].Arg[arg].RGBSrc == s->uTexture[tex].Arg[arg].AlphaSrc)
    {
        switch(s->uTexture[tex].Arg[arg].RGBSrc)
        {
            case WES_SRC_PREVIOUS:
                str += sprintf(str, "arg%i = gl_FragColor;\n", arg);  break;
            case WES_SRC_CONSTANT:
                str += sprintf(str, "arg%i = uTexture[%i].EnvColor;\n", arg, tex);break;
            case WES_SRC_PRIMARY_COLOR:
                str += sprintf(str, "arg%i = vColor;\n", arg); break;
            case WES_SRC_TEXTURE:
                str += sprintf(str, "arg%i = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy);\n", arg, tex, tex); break;
            default:{
                int ind = s->uTexture[tex].Arg[arg].RGBSrc - WES_SRC_TEXTURE0;
                if (ind > 0 && ind < WES_MULTITEX_NUM){
                    str += sprintf(str, "arg%i = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy);\n", arg, ind, tex);
                }
                break;
            }
        }
    } else {

        switch(s->uTexture[tex].Arg[arg].RGBSrc)
        {
            case WES_SRC_PREVIOUS:
                str += sprintf(str, "arg%i.rgb = gl_FragColor.rgb;\n", arg);break;
            case WES_SRC_CONSTANT:
                str += sprintf(str, "arg%i.rgb = uTexture[%i].EnvColor.rgb;\n", arg, tex);break;
            case WES_SRC_PRIMARY_COLOR:
                str += sprintf(str, "arg%i.rgb = vColor.rgb;\n", arg);break;
            case WES_SRC_TEXTURE:
                str += sprintf(str, "arg%i.rgb = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy).rgb;\n", arg, tex, tex);break;
            default:{
                int ind = s->uTexture[tex].Arg[arg].RGBSrc - WES_SRC_TEXTURE0;
                if (ind > 0 && ind < WES_MULTITEX_NUM){
                    str += sprintf(str, "arg%i.rgb = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy).rgb;\n", arg, ind, tex);
                }
                break;
            }
        }

        switch(s->uTexture[tex].Arg[arg].AlphaSrc)
        {
            case WES_SRC_PREVIOUS:
                str += sprintf(str, "arg%i.a = gl_FragColor.a;\n", arg);break;
            case WES_SRC_CONSTANT:
                str += sprintf(str, "arg%i.a = uTexture[%i].EnvColor.a;\n", arg, tex);break;
            case WES_SRC_PRIMARY_COLOR:
                str += sprintf(str, "arg%i.a = vColor.a;\n", arg);break;
            case WES_SRC_TEXTURE:
                str += sprintf(str, "arg%i.a = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy).a;\n", arg, tex, tex);break;
            default:{
                int ind = s->uTexture[tex].Arg[arg].RGBSrc - WES_SRC_TEXTURE0;
                if (ind > 0 && ind < WES_MULTITEX_NUM){
                    str += sprintf(str, "arg%i.a = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy).a;\n", arg, ind, tex);
                }
                break;
            }
        }
    }

    str += wes_frag_op(str, s, tex, arg);
    return (GLint)(str - buff);
}

GLint
wes_frag_combine(char *buff, progstate_t *s, int tex)
{
    char *str  = buff;

    if (s->uTexture[tex].RGBCombine == s->uTexture[tex].AlphaCombine)
    {
        switch(s->uTexture[tex].RGBCombine)
        {
            case WES_FUNC_REPLACE:
                str += wes_frag_arg(str, s, tex, 0);
                str += sprintf(str, "gl_FragColor = arg0;\n");
                break;

            case WES_FUNC_MODULATE:
                str += wes_frag_arg(str, s, tex, 0);
                str += wes_frag_arg(str, s, tex, 1);
                str += sprintf(str, "gl_FragColor = arg0 * arg1;\n");
                break;

            case WES_FUNC_ADD:
                str += wes_frag_arg(str, s, tex, 0);
                str += wes_frag_arg(str, s, tex, 1);
                str += sprintf(str, "gl_FragColor = arg0 + arg1;\n");
                break;

            case WES_FUNC_ADD_SIGNED:
                str += wes_frag_arg(str, s, tex, 0);
                str += wes_frag_arg(str, s, tex, 1);
                str += sprintf(str, "gl_FragColor = arg0 + arg1 - vec4(0.5,0.5,0.5,0.5);\n");
                break;

            case WES_FUNC_INTERPOLATE:
                str += wes_frag_arg(str, s, tex, 0);
                str += wes_frag_arg(str, s, tex, 1);
                str += wes_frag_arg(str, s, tex, 2);
                str += sprintf(str, "gl_FragColor = arg0 * arg2 + arg1 * (vec4(1.0, 1.0, 1.0, 1.0) - arg2);\n");
                break;

            case WES_FUNC_SUBTRACT:
                str += wes_frag_arg(str, s, tex, 0);
                str += wes_frag_arg(str, s, tex, 1);
                str += sprintf(str, "gl_FragColor = arg0 - arg1;\n");
                break;
        }

    } else if (s->uTexture[tex].RGBCombine == WES_FUNC_DOT3_RGBA){
        str += wes_frag_arg(str, s, tex, 0);
        str += wes_frag_arg(str, s, tex, 1);
        str += sprintf(str, "lowp vec3 col0 = arg0.rgb - vec3(0.5,0.5,0.5);\n");
        str += sprintf(str, "lowp vec3 col1 = arg1.rgb - vec3(0.5,0.5,0.5);\n");
        str += sprintf(str, "lowp float f = 4 * dot(col0, col1);\n");
        str += sprintf(str, "gl_FragColor = vec4(f, f, f, f);\n");
    } else {

        str += wes_frag_arg(str, s, tex, 0);
        str += wes_frag_arg(str, s, tex, 1);
        if (s->uTexture[tex].RGBCombine == WES_FUNC_INTERPOLATE ||
            s->uTexture[tex].AlphaCombine == WES_FUNC_INTERPOLATE){
            str += wes_frag_arg(str, s, tex, 2);
        }

        switch(s->uTexture[tex].RGBCombine)
        {
            case WES_FUNC_REPLACE:
                str += sprintf(str, "gl_FragColor.rgb = arg0.rgb;\n");
                break;

            case WES_FUNC_MODULATE:
                str += sprintf(str, "gl_FragColor.rgb = arg0.rgb * arg1.rgb;\n");
                break;

            case WES_FUNC_ADD:
                str += sprintf(str, "gl_FragColor.rgb = arg0.rgb + arg1.rgb;\n");
                break;

            case WES_FUNC_ADD_SIGNED:
                str += sprintf(str, "gl_FragColor.rgb = arg0.rgb + arg1.rgb - vec3(0.5,0.5,0.5);\n");
                break;

            case WES_FUNC_INTERPOLATE:
                str += sprintf(str, "gl_FragColor.rgb = arg0.rgb * arg2.rgb + arg1.rgb * (vec3(1.0, 1.0, 1.0) - arg2.rgb);\n");
                break;

            case WES_FUNC_SUBTRACT:
                str += sprintf(str, "gl_FragColor.rgb = arg0.rgb - arg1.rgb;\n");
                break;

            case WES_FUNC_DOT3_RGB:
                str += sprintf(str, "lowp vec3 col0 = arg0.rgb - vec3(0.5,0.5,0.5);\n");
                str += sprintf(str, "lowp vec3 col1 = arg1.rgb - vec3(0.5,0.5,0.5);\n");
                str += sprintf(str, "lowp float f = 4 * dot(col0, col1);\n");
                str += sprintf(str, "gl_FragColor.rgb = vec3(f,f,f);\n");
                break;
        }


        switch(s->uTexture[tex].AlphaCombine)
        {
            case WES_FUNC_REPLACE:
                str += sprintf(str, "gl_FragColor.a = arg0.a;\n");
                break;

            case WES_FUNC_MODULATE:
                str += sprintf(str, "gl_FragColor.a = arg0.a * arg1.a;\n");
                break;

            case WES_FUNC_ADD:
                str += sprintf(str, "gl_FragColor.a = arg0.a + arg1.a;\n");
                break;

            case WES_FUNC_ADD_SIGNED:
                str += sprintf(str, "gl_FragColor.a = arg0.a + arg1.a - 0.5;\n");
                break;

            case WES_FUNC_INTERPOLATE:
                str += sprintf(str, "gl_FragColor.a = arg0.a * arg2.a + arg1.a * (1.0 - arg2.a);\n");
                break;

            case WES_FUNC_SUBTRACT:
                str += sprintf(str, "gl_FragColor.a = arg0.a - arg1.a;\n");
                break;
        }

    }

    return (GLint)(str - buff);
}

GLint
wes_frag_tex(char* buff, progstate_t *s)
{
    char *str = buff;
    str += sprintf(str, "lowp vec4 arg0, arg1, arg2;\n");

    GLint i;
    for(i = 0; i < WES_MULTITEX_NUM; i++)
    {
        switch(s->uTexture[i].Mode)
        {
            case WES_FUNC_REPLACE:
                str += sprintf(str, "gl_FragColor = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy);\n", i, i);
                break;

            case WES_FUNC_MODULATE:
                str += sprintf(str, "gl_FragColor *= texture2D(uTexture[%i].Unit, vTexCoord[%i].xy);\n", i, i);
                break;

            case WES_FUNC_DECAL:
                str += sprintf(str, "lowp vec4 col = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy);\n", i, i);
                str += sprintf(str, "gl_FragColor.rgb = (1.0 - col.a) * gl_FragColor.rgb + col.a * col.rgb;\n");
                break;

            case WES_FUNC_BLEND:
                str += sprintf(str, "lowp vec4 col = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy);\n", i, i);
                str += sprintf(str, "gl_FragColor.rgb = (vec3(1.0,1.0,1.0) - col.rgb) * gl_FragColor.rgb + col.rgb * uTexture[0].EnvColor.rgb;\n");
                str += sprintf(str, "gl_FragColor.a *= col.a;\n");
                break;

            case WES_FUNC_ADD:
                str += sprintf(str, "lowp vec4 col = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy);\n", i, i);
                str += sprintf(str, "gl_FragColor.rgb += col.rgb;\n");
                str += sprintf(str, "gl_FragColor.a *= col.a;\n");
                break;

            case WES_FUNC_COMBINE:
                str += wes_frag_combine(str, s, i);
                break;
        }
    }
    return (GLint)(str - buff);
}


GLvoid
wes_frag_build(char* buff, progstate_t *s)
{
    char *str = buff;
    str += sprintf(str, "%s", frag_header);
    str += wes_frag_tex(str, s);
    str += wes_frag_fog(str, s);
    str += wes_frag_clipplane(str, s);
    str += wes_frag_alphatest(str, s);
    str += sprintf(str, "}\n");
}
