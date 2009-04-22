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

GLvoid
wes_halveimage(GLint nw, GLint nh, GLint byteperpixel, char* data, char* newdata)
{
    int i, j;
    for(i = 0; i < nw; i++){
        for(j = 0; j < nh; j++){
            memcpy(&newdata[(i + j * nw) * byteperpixel], &data[(i + j * nw *2) * 2*byteperpixel], byteperpixel);
        }
    }
}

GLvoid
gluBuild2DMipmaps(GLenum target, GLint components, GLsizei width, GLsizei height,
                GLenum format, GLenum type, const GLvoid *pixels )
{
    int i = 1;
    GLuint byteperpixel = 0;
    char *data = NULL, *newdata = NULL;
    switch(type)
    {
        case GL_UNSIGNED_BYTE:
            byteperpixel = components;
            break;

        case GL_UNSIGNED_SHORT_4_4_4_4:
        case GL_UNSIGNED_SHORT_5_5_5_1:
        case GL_UNSIGNED_SHORT_5_6_5:
            byteperpixel = 2;
            break;
    }

    if (byteperpixel == 0)
    {
        return;
    }

    glTexImage2D(target, 0, format, width, height, 0, format, type, pixels);

    data = malloc(width * height * byteperpixel);
    memcpy(data, pixels, width * height * byteperpixel);
    while(width != 1 || height != 1){
        width  >>= 1;
        height >>= 1;
        if (width == 0) width = 1;
        if (height == 0) height = 1;
        newdata = malloc(width * height * byteperpixel);
        wes_halveimage(width, height, byteperpixel, data, newdata);
        glTexImage2D(target, i++, format, width, height, 0, format, type, newdata);
        free(data);
        data = newdata;
    }

    if (newdata != NULL)
        free(newdata);
}
