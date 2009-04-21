#include <stdlib.h>
#include "wes.h"
#include "wes_gl_defines.h"

GLvoid
wes_convert_BGR2RGB(const GLubyte* inb, GLubyte* outb, GLint size)
{
    int i;
    for(i = 0; i < size; i += 3){
        outb[i + 2] = inb[i];
        outb[i + 1] = inb[i + 1];
        outb[i] = inb[i + 2];
    }
}

GLvoid
wes_convert_BGRA2RGBA(const GLubyte* inb, GLubyte* outb, GLint size)
{
    int i;
    for(i = 0; i < size; i += 4){
        outb[i + 2] = inb[i];
        outb[i + 1] = inb[i + 1];
        outb[i] = inb[i + 2];
        outb[i + 3] = inb[i + 3];
    }
}

GLvoid
wes_convert_I2LA(const GLubyte* inb, GLubyte* outb, GLint size)
{
    int i;
    for(i = 0; i < size; i += 1){
        outb[i*2 + 1] = outb[i*2] = inb[i];
    }
}


GLvoid
glTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height,
           GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    /* conversion routines */
    if (format == GL_BGR){
        GLvoid* data = malloc(width * height * 3);
        wes_convert_BGR2RGB((GLubyte*) pixels, (GLubyte*) data, width * height * 3);
        wes_gl->glTexImage2D(target, level, GL_RGB, width, height, 0, GL_RGB, type, data);
        free(data);
    } else if (format == GL_BGRA){
        GLvoid* data = malloc(width * height * 4);
        wes_convert_BGRA2RGBA((GLubyte*) pixels, (GLubyte*) data, width * height * 4);
        wes_gl->glTexImage2D(target, level, GL_RGBA, width, height, 0, GL_RGBA, type, data);
        free(data);
    } else if (format == GL_INTENSITY){
        GLvoid* data = malloc(width * height * 2);
        wes_convert_I2LA(pixels, data, width * height * 2);
        wes_gl->glTexImage2D(target, level, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, type, data);
        free(data);
    } else {
        wes_gl->glTexImage2D(target, level, format, width, height, 0, format, type, pixels);
    }
}

