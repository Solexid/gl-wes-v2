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
wes_frag_combine(char* buff, progstate_t *s, int i)
{


}


GLint
wes_frag_tex(char* buff, progstate_t *s)
{
    char *str = buff;

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
                str += sprintf(str, "gl_FragColor.rgb = (1.0 - col.a) * gl_FragColor.rgb + col.a * col.rgb;");
                break;

            case WES_FUNC_BLEND:
                str += sprintf(str, "lowp vec4 col = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy);\n", i, i);
                str += sprintf(str, "gl_FragColor.rgb = (vec3(1.0,1.0,1.0) - col.rgb) * gl_FragColor.rgb + col.rgb * uTexture[0].EnvColor.rgb;");
                str += sprintf(str, "gl_FragColor.a *= col.a;");
                break;

            case WES_FUNC_ADD:
                str += sprintf(str, "lowp vec4 col = texture2D(uTexture[%i].Unit, vTexCoord[%i].xy);\n", i, i);
                str += sprintf(str, "gl_FragColor.rgb += col.rgb;");
                str += sprintf(str, "gl_FragColor.a *= col.a;");
                break;

            case WES_FUNC_COMBINE:
                wes_frag_combine(str, s, i);
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
