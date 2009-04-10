#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wes.h"
#include "wes_shader.h"

#define wes_setu1i(ind, x)       wes_gl->glUniform1i(sh_uniformloc[ind], x)
#define wes_setu2i(ind, x, y)    wes_gl->glUniform2i(sh_uniformloc[ind], x, y)
#define wes_setu1f(ind, x)       wes_gl->glUniform1f(sh_uniformloc[ind], x)
#define wes_setu2f(ind, x, y)    wes_gl->glUniform2f(sh_uniformloc[ind], x, y)

//shader global variables:
GLuint          sh_program;
GLuint          sh_fragment;
GLuint          sh_vertex;

sUniformLoc     sh_uloc;

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
wes_shader_create(char* fname, GLenum type)
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

    //Compile:
    index = wes_gl->glCreateShader(type);
    wes_gl->glShaderSource(index, 1, &data, NULL);
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
wes_setattribloc(GLuint prog)
{
    wes_gl->glBindAttribLocation(prog, WES_APOS,       "aPosition");
    wes_gl->glBindAttribLocation(prog, WES_ATEXCOORD0, "aTexCoord0");
    wes_gl->glBindAttribLocation(prog, WES_ATEXCOORD1, "aTexCoord1");
    wes_gl->glBindAttribLocation(prog, WES_ANORMAL,    "aNormal");
    wes_gl->glBindAttribLocation(prog, WES_AFOGCOORD,  "aFogCoord");
    wes_gl->glBindAttribLocation(prog, WES_ACOLOR0, "aColor0");
    wes_gl->glBindAttribLocation(prog, WES_ACOLOR1,    "aColor1");
}

GLvoid
wes_setuniformloc(GLuint prog)
{
    int i;
    char str[32];

    sh_uloc.uEnableRescaleNormal    = wes_gl->glGetUniformLocation(prog, "uEnableRescaleNormal" );
    sh_uloc.uEnableNormalize        = wes_gl->glGetUniformLocation(prog, "uEnableNormalize" );
    sh_uloc.uEnableTextureGenS      = wes_gl->glGetUniformLocation(prog, "uEnableTextureGenS");
    sh_uloc.uEnableTextureGenT      = wes_gl->glGetUniformLocation(prog, "uEnableTextureGenT");
    sh_uloc.uEnableTextureGenR      = wes_gl->glGetUniformLocation(prog, "uEnableTextureGenR");
    sh_uloc.uEnableTextureGenQ      = wes_gl->glGetUniformLocation(prog, "uEnableTextureGenQ");
    for(i = 0; i != 6; i++){
        sprintf(str, "uEnableClipPlane[%i]", i);
        sh_uloc.uEnableClipPlane[i] = wes_gl->glGetUniformLocation(prog, str);
    }
    sh_uloc.uEnableLighting         = wes_gl->glGetUniformLocation(prog, "uEnableLighting");
    for(i = 0; i != 8; i++){
        sprintf(str, "uEnableLight[%i]", i);
        sh_uloc.uEnableLight[i]     = wes_gl->glGetUniformLocation(prog, str);
    }
    sh_uloc.uEnableColorMaterial    = wes_gl->glGetUniformLocation(prog, "uEnableColorMaterial" );
    sh_uloc.uEnableFog              = wes_gl->glGetUniformLocation(prog, "uEnableFog");
    sh_uloc.uEnableAlphaTest        = wes_gl->glGetUniformLocation(prog, "uEnableAlphaTest");
    sh_uloc.uEnableColor0           = wes_gl->glGetUniformLocation(prog, "uEnableColor0");
    sh_uloc.uEnableTexture0         = wes_gl->glGetUniformLocation(prog, "uEnableTexture0");
    sh_uloc.uEnableFogCoord         = wes_gl->glGetUniformLocation(prog, "uEnableFogCoord");

    for(i = 0; i != 8; i++){
        sprintf(str, "uLight[%i].Type", i);
        sh_uloc.uLight[i].Type      = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].DistAttenuation", i);
        sh_uloc.uLight[i].DistAttenuation = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].Position", i);
        sh_uloc.uLight[i].Position = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].Attenuation[0]", i);
        sh_uloc.uLight[i].Attenuation[0] = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].Attenuation[1]", i);
        sh_uloc.uLight[i].Attenuation[1] = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].Attenuation[2]", i);
        sh_uloc.uLight[i].Attenuation[2] = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].ColorAmbient", i);
        sh_uloc.uLight[i].ColorAmbient  = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].ColorDiffuse", i);
        sh_uloc.uLight[i].ColorDiffuse  = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].ColorSpec", i);
        sh_uloc.uLight[i].ColorSpec  = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].SpotDir", i);
        sh_uloc.uLight[i].SpotDir  = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].SpotExponent", i);
        sh_uloc.uLight[i].SpotExponent  = wes_gl->glGetUniformLocation(prog, str);
        sprintf(str, "uLight[%i].SpotCutoff", i);
        sh_uloc.uLight[i].SpotCutoff  = wes_gl->glGetUniformLocation(prog, str);
    }

    sh_uloc.uMaterial.ColorAmbient  = wes_gl->glGetUniformLocation(prog, "uMaterial.ColorAmbient");
    sh_uloc.uMaterial.ColorDiffuse  = wes_gl->glGetUniformLocation(prog, "uMaterial.ColorDiffuse");
    sh_uloc.uMaterial.ColorSpec     = wes_gl->glGetUniformLocation(prog, "uMaterial.ColorSpec");
    sh_uloc.uMaterial.SpecExponent  = wes_gl->glGetUniformLocation(prog, "uMaterial.SpecExponent");

    sh_uloc.uFogType    = wes_gl->glGetUniformLocation(prog, "uFogType");
    sh_uloc.uFogDensity = wes_gl->glGetUniformLocation(prog, "uFogDensity");
    sh_uloc.uFogStart   = wes_gl->glGetUniformLocation(prog, "uFogStart");
    sh_uloc.uFogEnd     = wes_gl->glGetUniformLocation(prog, "uFogEnd");
    sh_uloc.uFogColor   = wes_gl->glGetUniformLocation(prog, "uFogColor");

    for(i = 0; i != 6; i++){
        sprintf(str, "uClipPlane[%i]", i);
        sh_uloc.uClipPlane[i] = wes_gl->glGetUniformLocation(prog, str);
    }

    sh_uloc.uMVP    = wes_gl->glGetUniformLocation(prog, "uMVP");
    sh_uloc.uMV     = wes_gl->glGetUniformLocation(prog, "uMV");
    sh_uloc.uMVinv  = wes_gl->glGetUniformLocation(prog, "uMVinv");

    sh_uloc.uAlphaFunc  = wes_gl->glGetUniformLocation(prog, "uAlphaFunc");
    sh_uloc.uAlphaRef   = wes_gl->glGetUniformLocation(prog, "uAlphaRef");
    sh_uloc.uTexture0   = wes_gl->glGetUniformLocation(prog, "uTexture0");
}

GLvoid
wes_setstate(GLenum e, GLboolean b)
{
    switch(e)
    {
        case GL_RESCALE_NORMAL:     wes_gl->glUniform1i(sh_uloc.uEnableRescaleNormal, b);   break;
        case GL_NORMALIZE:          wes_gl->glUniform1i(sh_uloc.uEnableNormalize, b);       break;
        case GL_TEXTURE_GEN_S:      wes_gl->glUniform1i(sh_uloc.uEnableTextureGenS, b);     break;
        case GL_TEXTURE_GEN_T:      wes_gl->glUniform1i(sh_uloc.uEnableTextureGenT, b);     break;
        case GL_TEXTURE_GEN_R:      wes_gl->glUniform1i(sh_uloc.uEnableTextureGenR, b);     break;
        case GL_TEXTURE_GEN_Q:      wes_gl->glUniform1i(sh_uloc.uEnableTextureGenQ, b);     break;

        case GL_CLIP_PLANE0:        wes_gl->glUniform1i(sh_uloc.uEnableClipPlane[0], b);    break;
        case GL_CLIP_PLANE1:        wes_gl->glUniform1i(sh_uloc.uEnableClipPlane[1], b);    break;
        case GL_CLIP_PLANE2:        wes_gl->glUniform1i(sh_uloc.uEnableClipPlane[2], b);    break;
        case GL_CLIP_PLANE3:        wes_gl->glUniform1i(sh_uloc.uEnableClipPlane[3], b);    break;
        case GL_CLIP_PLANE4:        wes_gl->glUniform1i(sh_uloc.uEnableClipPlane[4], b);    break;
        case GL_CLIP_PLANE5:        wes_gl->glUniform1i(sh_uloc.uEnableClipPlane[5], b);    break;

        case GL_LIGHTING:           wes_gl->glUniform1i(sh_uloc.uEnableLighting, b);        break;
        case GL_LIGHT0:             wes_gl->glUniform1i(sh_uloc.uEnableLight[0], b);        break;
        case GL_LIGHT1:             wes_gl->glUniform1i(sh_uloc.uEnableLight[1], b);        break;
        case GL_LIGHT2:             wes_gl->glUniform1i(sh_uloc.uEnableLight[2], b);        break;
        case GL_LIGHT3:             wes_gl->glUniform1i(sh_uloc.uEnableLight[3], b);        break;
        case GL_LIGHT4:             wes_gl->glUniform1i(sh_uloc.uEnableLight[4], b);        break;
        case GL_LIGHT5:             wes_gl->glUniform1i(sh_uloc.uEnableLight[5], b);        break;
        case GL_LIGHT6:             wes_gl->glUniform1i(sh_uloc.uEnableLight[6], b);        break;
        case GL_LIGHT7:             wes_gl->glUniform1i(sh_uloc.uEnableLight[7], b);        break;
        case GL_COLOR_MATERIAL:     wes_gl->glUniform1i(sh_uloc.uEnableColorMaterial, b);   break;

        case GL_FOG:                wes_gl->glUniform1i(sh_uloc.uEnableFog, b);             break;
        case GL_ALPHA_TEST:         wes_gl->glUniform1i(sh_uloc.uEnableAlphaTest, b);       break;

        default:                    wes_gl->glEnable(e); break;
    }
}


GLvoid
glLightfv(GLenum light, GLenum pname, GLfloat* params)
{
    GLuint ind = light - GL_LIGHT0;
    switch(pname)
    {
        case GL_AMBIENT:
            wes_gl->glUniform4fv(sh_uloc.uLight[ind].ColorAmbient, 1, params); break;
        case GL_DIFFUSE:
            wes_gl->glUniform4fv(sh_uloc.uLight[ind].ColorDiffuse, 1, params); break;
        case GL_SPECULAR:
            wes_gl->glUniform4fv(sh_uloc.uLight[ind].ColorSpec, 1, params); break;
        case GL_POSITION:
            wes_gl->glUniform4fv(sh_uloc.uLight[ind].Position, 1, params); break;
        case GL_SPOT_DIRECTION:
            wes_gl->glUniform3fv(sh_uloc.uLight[ind].SpotDir, 1, params); break;
        case GL_SPOT_EXPONENT:
            wes_gl->glUniform1fv(sh_uloc.uLight[ind].SpotExponent, 1, params); break;
        case GL_SPOT_CUTOFF:
            wes_gl->glUniform1fv(sh_uloc.uLight[ind].SpotCutoff, 1, params); break;
        case GL_CONSTANT_ATTENUATION:
            wes_gl->glUniform1fv(sh_uloc.uLight[ind].Attenuation[0], 1, params); break;
        case GL_LINEAR_ATTENUATION:
            wes_gl->glUniform1fv(sh_uloc.uLight[ind].Attenuation[1], 1, params); break;
        case GL_QUADRATIC_ATTENUATION:
            wes_gl->glUniform1fv(sh_uloc.uLight[ind].Attenuation[2], 1, params); break;
    }
}

GLvoid
glMaterialfv(GLenum face, GLenum pname, GLfloat* params)
{
    switch(pname)
    {
        case GL_AMBIENT:
            wes_gl->glUniform4fv(sh_uloc.uMaterial.ColorAmbient, 1, params); break;
        case GL_DIFFUSE:
            wes_gl->glUniform4fv(sh_uloc.uMaterial.ColorDiffuse, 1, params); break;
        case GL_AMBIENT_AND_DIFFUSE:
            wes_gl->glUniform4fv(sh_uloc.uMaterial.ColorAmbient, 1, params); break;
        case GL_SPECULAR:
            wes_gl->glUniform4fv(sh_uloc.uMaterial.ColorSpec, 1, params); break;
        case GL_EMISSION:
            wes_gl->glUniform4fv(sh_uloc.uMaterial.ColorEmissive, 1, params); break;
        case GL_SHININESS:
            wes_gl->glUniform1fv(sh_uloc.uMaterial.SpecExponent, 1, params); break;
    }
}

/*
GLvoid
glLightModel(GLenum pname, GLfloat* params)
{
    switch(pname)
    {
        case GL_LIGHT_MODEL_AMBIENT:
            wes_gl->glUniform4fv(sh_uloc.uMaterial.ColorAmbient, 1, params); break;
        case GL_DIFFUSE:
            wes_gl->glUniform4fv(sh_uloc.uMaterial.ColorDiffuse, 1, params); break;
        case GL_AMBIENT_AND_DIFFUSE:
            wes_gl->glUniform4fv(sh_uloc.uMaterial.ColorAmbient, 1, params); break;
        case GL_SPECULAR:
            wes_gl->glUniform4fv(sh_uloc.uMaterial.ColorSpec, 1, params); break;
        case GL_EMISSION:
            wes_gl->glUniform4fv(sh_uloc.uMaterial.ColorEmissive, 1, params); break;
        case GL_SHININESS:
            wes_gl->glUniform1fv(sh_uloc.uMaterial.SpecExponent, 1, params); break;
    }
}
*/

GLvoid
glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    wes_gl->glVertexAttribPointer(WES_APOS, size, type, GL_FALSE, stride, ptr);
}

GLvoid
glNormalPointer(GLenum type, GLsizei stride, const GLvoid *ptr)
{
    wes_gl->glVertexAttribPointer(WES_ANORMAL, 3, type, GL_FALSE, stride, ptr);
}

GLvoid
glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    wes_gl->glVertexAttribPointer(WES_ACOLOR0, size, type, GL_FALSE, stride, ptr);
}

GLvoid
glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    wes_gl->glVertexAttribPointer(WES_ATEXCOORD0, size, type, GL_FALSE, stride, ptr);
}

GLvoid
glSecondaryColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    wes_gl->glVertexAttribPointer(WES_ACOLOR1, size, type, GL_FALSE, stride, ptr);
}

GLvoid
glFogCoordPointer(GLenum type, GLsizei stride, const GLvoid *ptr)
{
    wes_gl->glVertexAttribPointer(WES_AFOGCOORD, 1, type, GL_FALSE, stride, ptr);
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
    wes_gl->glUniform1i(sh_uloc.uAlphaFunc, func);
    wes_gl->glUniform1f(sh_uloc.uAlphaRef, ref);
}

GLvoid
glFogi(GLenum pname, GLint param)
{
    switch(pname)
    {
        case GL_FOG_MODE:
            if (param == GL_LINEAR)
                wes_gl->glUniform1i(sh_uloc.uFogType, 0);

            break;
        case GL_FOG_COORD_SRC:
            wes_gl->glUniform1i(sh_uloc.uEnableFogCoord, (param == GL_FOG_COORD)); break;
    }
}

GLvoid
glFogf(GLenum pname, GLfloat param)
{
    switch(pname)
    {
        case GL_FOG_DENSITY:
            wes_gl->glUniform1f(sh_uloc.uFogDensity, param); break;
        case GL_FOG_START:
            wes_gl->glUniform1f(sh_uloc.uFogStart, param); break;
        case GL_FOG_END:
            wes_gl->glUniform1f(sh_uloc.uFogEnd, param); break;
    }
}

GLvoid
glFogfv(GLenum pname, GLfloat *param)
{
    switch(pname)
    {
        case GL_FOG_COLOR:
            wes_gl->glUniform4fv(sh_uloc.uFogColor, 1, param); break;
    }
}

GLvoid wes_setinitialstate()
{
    glDisable(GL_RESCALE_NORMAL);
    glDisable(GL_NORMALIZE);
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
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
    wes_setattribloc(prog);
    wes_setuniformloc(prog);
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
wes_shader_init()
{
    sh_fragment = wes_shader_create("WES.fsh", GL_FRAGMENT_SHADER);
    sh_vertex   = wes_shader_create("WES.vsh", GL_VERTEX_SHADER);
    sh_program  = wes_program_create(sh_fragment, sh_vertex);

    wes_setinitialstate();
    wes_gl->glUseProgram(sh_program);
}

GLvoid
wes_shader_destroy()
{

}
