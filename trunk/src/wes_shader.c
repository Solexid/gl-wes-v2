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
#include "wes_shader.h"
#include "wes_matrix.h"

#define WES_PBUFFER_SIZE    128

//shader global variables:
program_t       *sh_program;
GLboolean       sh_program_mod;
program_t       sh_pbuffer[WES_PBUFFER_SIZE];
GLuint          sh_pbuffer_count;
GLuint          sh_vertex;

//function declarations:
GLvoid
wes_shader_error(GLuint ind)
{
    int len;
    char* log;
    wes_gl->glGetShaderiv(ind, GL_INFO_LOG_LENGTH, &len);
    log = (char*) malloc(len * sizeof(char));
    wes_gl->glGetShaderInfoLog(ind, len, NULL, log);
    PRINT_ERROR("\nShader Error: %s", log);
    free(log);
}

GLvoid
wes_program_error(GLuint ind)
{
    int len;
    char* log;
    wes_gl->glGetProgramiv(ind, GL_INFO_LOG_LENGTH, &len);
    log = (char*) malloc(len * sizeof(char));
    wes_gl->glGetProgramInfoLog(ind, len, NULL, log);
    PRINT_ERROR("\nProgram Error: %s", log);
    free(log);
}

GLuint
wes_shader_create(char* fname, GLenum type, char* defines)
{
    FILE*   file;
    int     size;
    char*   data;
    GLuint  index;
    GLint   success;

    //Load file into mem:
    file = fopen(fname, "rb");
	if (!file){
	    PRINT_ERROR("Could not find file: %s", fname);
		return (-1);
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	data = (char*) malloc(size + 1);
	if (!data){
	    PRINT_ERROR("Could not allocate: %i bytes", size + 1);
    	return (-1);
    }
	if (fread(data, sizeof(char), size, file) != size){
        free(data);
        PRINT_ERROR("Could not read file: %s", fname);
		return (-1);
	}
	data[size] = '\0';
	fclose(file);

    char *src[2];
    src[0] = defines;
    src[1] = data;

    //Compile:
    index = wes_gl->glCreateShader(type);
    wes_gl->glShaderSource(index, 2, (GLvoid*) src, NULL);
    wes_gl->glCompileShader(index);
    free(data);

    //test status:
    wes_gl->glGetShaderiv(index, GL_COMPILE_STATUS, &success);
    if (success){
        return index;
    } else {
        wes_shader_error(index);
        wes_gl->glDeleteShader(index);
        return (-1);
    }

}


GLvoid
wes_attrib_loc(GLuint prog)
{
    wes_gl->glBindAttribLocation(prog, WES_APOS,       "aPosition");
    wes_gl->glBindAttribLocation(prog, WES_ATEXCOORD0, "aTexCoord0");
    wes_gl->glBindAttribLocation(prog, WES_ATEXCOORD1, "aTexCoord1");
    wes_gl->glBindAttribLocation(prog, WES_ATEXCOORD2, "aTexCoord2");
    wes_gl->glBindAttribLocation(prog, WES_ATEXCOORD3, "aTexCoord3");
    wes_gl->glBindAttribLocation(prog, WES_ANORMAL,    "aNormal");
    wes_gl->glBindAttribLocation(prog, WES_AFOGCOORD,  "aFogCoord");
    wes_gl->glBindAttribLocation(prog, WES_ACOLOR0,     "aColor");
    wes_gl->glBindAttribLocation(prog, WES_ACOLOR1,    "aColor2nd");
}



GLvoid
wes_uniform_loc(program_t *p)
{
#define LocateUniform(A)                                                \
    p->uloc.A = wes_gl->glGetUniformLocation(p->prog, #A)

#define LocateUniformIndex(A, B, I)                                    \
    sprintf(str, #A "[%i]" #B, I);                                     \
    p->uloc.A[I]B = wes_gl->glGetUniformLocation(p->prog, str)

    int i;
    char str[32];

    LocateUniform(uEnableRescaleNormal);
    LocateUniform(uEnableNormalize);
    for(i = 0; i != WES_MULTITEX_NUM; i++)  {
        LocateUniformIndex(uEnableTextureGen, ,i);
    }
    for(i = 0; i != WES_CLIPPLANE_NUM; i++){
        LocateUniformIndex(uEnableClipPlane, ,i);
    }

    LocateUniform(uEnableFog);
    LocateUniform(uEnableAlphaTest);
    LocateUniform(uEnableFogCoord);
    LocateUniform(uEnableLighting);
    for(i = 0; i != WES_LIGHT_NUM; i++){
        LocateUniformIndex(uEnableLight, , i);
        LocateUniformIndex(uLight, .Position, i);
        LocateUniformIndex(uLight, .Attenuation, i);
        LocateUniformIndex(uLight, .ColorAmbient, i);
        LocateUniformIndex(uLight, .ColorDiffuse, i);
        LocateUniformIndex(uLight, .ColorSpec, i);
        LocateUniformIndex(uLight, .SpotDir, i);
        LocateUniformIndex(uLight, .SpotVar, i);
    }

    LocateUniform(uLightModel.ColorAmbient);
    LocateUniform(uLightModel.TwoSided);
    LocateUniform(uLightModel.LocalViewer);
    LocateUniform(uLightModel.ColorControl);
    LocateUniform(uRescaleFactor);


    for(i = 0; i < 2; i++){
        LocateUniformIndex(uMaterial, .ColorAmbient, i);
        LocateUniformIndex(uMaterial, .ColorDiffuse, i);
        LocateUniformIndex(uMaterial, .ColorSpec, i);
        LocateUniformIndex(uMaterial, .ColorEmissive, i);
        LocateUniformIndex(uMaterial, .SpecExponent, i);
        LocateUniformIndex(uMaterial, .ColorMaterial, i);
    }

    LocateUniform(uFogMode);
    LocateUniform(uFogDensity);
    LocateUniform(uFogStart);
    LocateUniform(uFogEnd);
    LocateUniform(uFogColor);

    for(i = 0; i != WES_CLIPPLANE_NUM; i++){
        LocateUniformIndex(uClipPlane, ,i);
    }

    LocateUniform(uMVP);
    LocateUniform(uMV);
    LocateUniform(uMVIT);
    LocateUniform(uAlphaRef);

    for(i = 0; i != WES_MULTITEX_NUM; i++){
        LocateUniformIndex(uTexture, .Unit, i);
        LocateUniformIndex(uTexture, .EnvColor, i);
        /*
        sprintf(str, "uTexture[%i].Func", i);
        u_state.uTexture[i].Func.loc   = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uTexture[%i].Arg0", i);
        u_state.uTexture[i].Arg0.loc   = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uTexture[%i].Arg1", i);
        u_state.uTexture[i].Arg1.loc   = wes_gl->glGetUniformLocation(sh_program, str);
        sprintf(str, "uTexture[%i].Arg2", i);
        u_state.uTexture[i].Arg2.loc   = wes_gl->glGetUniformLocation(sh_program, str);
        */
    }

#undef LocateUniform
#undef LocateUniformIndex
}


GLuint
wes_program_create(GLuint frag, GLuint vert)
{
    GLuint  prog;
    GLint   success;

    //Create & attach
    prog = wes_gl->glCreateProgram();
    wes_gl->glAttachShader(prog, frag);
    wes_gl->glAttachShader(prog, vert);
    wes_gl->glLinkProgram(prog);
    wes_attrib_loc(prog);
    wes_gl->glLinkProgram(prog);

    //check status:
    wes_gl->glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (success || wes_gl->glGetError()){
        return prog;
    } else {
        wes_program_error(prog);
        wes_gl->glDeleteProgram(prog);
        return (-1);
    }
};



GLvoid
wes_build_program( progstate_t *s, program_t *p)
{
    char        state[1024];
    char        *str;

    str = state;
    str += sprintf(str, "#define DEF_ALPHA_TEST     \t %i \n", s->DEF_ALPHA_TEST);
    str += sprintf(str, "#define DEF_TEXTURE0_ENV   \t %i \n", s->DEF_TEXTURE0_ENV);
    str += sprintf(str, "#define DEF_TEXTURE1_ENV   \t %i \n", s->DEF_TEXTURE1_ENV);
    str += sprintf(str, "#define DEF_TEXTURE2_ENV   \t %i \n", s->DEF_TEXTURE2_ENV);
    str += sprintf(str, "#define DEF_TEXTURE3_ENV   \t %i \n", s->DEF_TEXTURE3_ENV);
    str += sprintf(str, "#define DEF_FOG            \t %i \n", s->DEF_FOG);
    str += sprintf(str, "#define DEF_CLIPPLANE      \t %i \n", s->DEF_CLIPPLANE);

    p->pstate = *s;
    p->vert = sh_vertex;
    p->frag = wes_shader_create("WES.fsh", GL_FRAGMENT_SHADER, state);
    p->prog = wes_program_create(p->frag, p->vert);
    wes_uniform_loc(p);
}

GLboolean
wes_progstate_cmp(progstate_t* s0, progstate_t* s1)
{
    if ((s0->DEF_ALPHA_TEST == s1->DEF_ALPHA_TEST) &&
        (s0->DEF_TEXTURE0_ENV == s1->DEF_TEXTURE0_ENV) &&
        (s0->DEF_TEXTURE1_ENV == s1->DEF_TEXTURE1_ENV) &&
        (s0->DEF_TEXTURE2_ENV == s1->DEF_TEXTURE2_ENV) &&
        (s0->DEF_TEXTURE3_ENV == s1->DEF_TEXTURE3_ENV) &&
        (s0->DEF_FOG == s1->DEF_FOG) &&
        (s0->DEF_CLIPPLANE == s1->DEF_CLIPPLANE)){
        return 0;
    }

    return 1;
}

GLvoid
wes_choose_program(progstate_t *s)
{
    int i;
    for(i = 0; i < sh_pbuffer_count; i++)
    {
        if (!wes_progstate_cmp(s, &sh_pbuffer[i].pstate))
        {
            if (sh_program != &sh_pbuffer[i]) {
                sh_program = &sh_pbuffer[i];
                sh_program_mod = GL_TRUE;
                }
            return;
        }
    }

    sh_program = &sh_pbuffer[sh_pbuffer_count];
    wes_build_program(s, sh_program);
    sh_pbuffer_count++;
    sh_program_mod = GL_TRUE;

}

GLvoid
wes_shader_init()
{
    sh_pbuffer_count = 0;
    sh_program_mod = GL_TRUE;
    sh_vertex = wes_shader_create("WES.vsh", GL_VERTEX_SHADER, "");
}


GLvoid
wes_shader_destroy()
{
    GLint i;
    wes_gl->glDeleteShader(sh_vertex);
    for(i = 0; i < sh_pbuffer_count; i++)
    {
        wes_gl->glDeleteShader(sh_pbuffer[i].frag);
        wes_gl->glDeleteProgram(sh_pbuffer[i].prog);
    }
}



