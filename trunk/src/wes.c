
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wes.h"
#include "wes_begin.h"
#include "wes_shader.h"
#include "wes_matrix.h"

#if defined(_WIN32)
    #include <windows.h>
    #define dlopen(A, B)    LoadLibrary(A)
    #define dlsym(A, B)     GetProcAddress(A, B)
    #define dlclose(A)      FreeLibrary(A)
    const char* wes_libname = "libGLESv2.dll";
#elif defined(POSIX)
    #include <dlfcn.h>
    const char* wes_libname = "libGLESv2.so";
#endif

void*           wes_libhandle = NULL;
sGLESv2Lib*     wes_gl = NULL;

//OpenGL ES 2 function names for runtime library loading:
const char* glfuncnames[] =
{
    "glActiveTexture",
    "glAttachShader",
    "glBindAttribLocation",
    "glBindBuffer",
    "glBindFramebuffer",
    "glBindRenderbuffer",
    "glBindTexture",
    "glBlendColor",
    "glBlendEquation",
    "glBlendEquationSeparate",
    "glBlendFunc",
    "glBlendFuncSeparate",
    "glBufferData",
    "glBufferSubData",
    "glCheckFramebufferStatus",
    "glClear",
    "glClearColor",
    "glClearDepthf",
    "glClearStencil",
    "glColorMask",
    "glCompileShader",
    "glCompressedTexImage2D",
    "glCompressedTexSubImage2D",
    "glCopyTexImage2D",
    "glCopyTexSubImage2D",
    "glCreateProgram",
    "glCreateShader",
    "glCullFace",
    "glDeleteBuffers",
    "glDeleteFramebuffers",
    "glDeleteTextures",
    "glDeleteProgram",
    "glDeleteRenderbuffers",
    "glDeleteShader",
    "glDetachShader",
    "glDepthFunc",
    "glDepthMask",
    "glDepthRangef",
    "glDisable",
    "glDisableVertexAttribArray",
    "glDrawArrays",
    "glDrawElements",
    "glEnable",
    "glEnableVertexAttribArray",
    "glFinish",
    "glFlush",
    "glFramebufferRenderbuffer",
    "glFramebufferTexture2D",
    "glFrontFace",
    "glGenBuffers",
    "glGenerateMipmap",
    "glGenFramebuffers",
    "glGenRenderbuffers",
    "glGenTextures",
    "glGetActiveAttrib",
    "glGetActiveUniform",
    "glGetAttachedShaders",
    "glGetAttribLocation",
    "glGetBooleanv",
    "glGetBufferParameteriv",
    "glGetError",
    "glGetFloatv",
    "glGetFramebufferAttachmentParameteriv",
    "glGetIntegerv",
    "glGetProgramiv",
    "glGetProgramInfoLog",
    "glGetRenderbufferParameteriv",
    "glGetShaderiv",
    "glGetShaderInfoLog",
    "glGetShaderPrecisionFormat",
    "glGetShaderSource",
    "glGetString",
    "glGetTexParameterfv",
    "glGetTexParameteriv",
    "glGetUniformfv",
    "glGetUniformiv",
    "glGetUniformLocation",
    "glGetVertexAttribfv",
    "glGetVertexAttribiv",
    "glGetVertexAttribPointerv",
    "glHint",
    "glIsBuffer",
    "glIsEnabled",
    "glIsFramebuffer",
    "glIsProgram",
    "glIsRenderbuffer",
    "glIsShader",
    "glIsTexture",
    "glLineWidth",
    "glLinkProgram",
    "glPixelStorei",
    "glPolygonOffset",
    "glReadPixels",
    "glReleaseShaderCompiler",
    "glRenderbufferStorage",
    "glSampleCoverage",
    "glScissor",
    "glShaderBinary",
    "glShaderSource",
    "glStencilFunc",
    "glStencilFuncSeparate",
    "glStencilMask",
    "glStencilMaskSeparate",
    "glStencilOp",
    "glStencilOpSeparate",
    "glTexImage2D",
    "glTexParameterf",
    "glTexParameterfv",
    "glTexParameteri",
    "glTexParameteriv",
    "glTexSubImage2D",
    "glUniform1f",
    "glUniform1fv",
    "glUniform1i",
    "glUniform1iv",
    "glUniform2f",
    "glUniform2fv",
    "glUniform2i",
    "glUniform2iv",
    "glUniform3f",
    "glUniform3fv",
    "glUniform3i",
    "glUniform3iv",
    "glUniform4f",
    "glUniform4fv",
    "glUniform4i",
    "glUniform4iv",
    "glUniformMatrix2fv",
    "glUniformMatrix3fv",
    "glUniformMatrix4fv",
    "glUseProgram",
    "glValidateProgram",
    "glVertexAttrib1f",
    "glVertexAttrib1fv",
    "glVertexAttrib2f",
    "glVertexAttrib2fv",
    "glVertexAttrib3f",
    "glVertexAttrib3fv",
    "glVertexAttrib4f",
    "glVertexAttrib4fv",
    "glVertexAttribPointer",
    "glViewport"
};


GLvoid
wes_init()
{
    int i;
    void** ptr;

    wes_gl = malloc(sizeof(sGLESv2Lib));
    if (wes_gl == NULL)
    {
        PRINT_ERROR("Could not load Allocate mem: %s", wes_libname);
    }

    wes_libhandle = dlopen(wes_libname, RTLD_LAZY);
    if (wes_libhandle == NULL)
    {
        PRINT_ERROR("Could not load OpenGL ES 2 runtime library: %s", wes_libname);
    }

    ptr = (void**) wes_gl;
    for(i = 0; i != WES_OGLESV2_FUNCTIONCOUNT+1; i++)
    {
        void* pfunc = dlsym(wes_libhandle, glfuncnames[i]);
        if (pfunc == NULL)
        {
            PRINT_ERROR("Could not find %s in %s", glfuncnames[i], wes_libname);
        }
        *ptr++ = pfunc;
    }

    wes_shader_init();
    wes_begin_init();
    wes_matrix_init();
}

GLvoid
wes_destroy()
{
    dlclose(wes_libhandle);
    wes_shader_destroy();
    wes_begin_destroy();
}
