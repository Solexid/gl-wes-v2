#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wes.h"
#include "wes_begin.h"
#include "wes_shader.h"
#include "wes_matrix.h"
#include "wes_uniform.h"

#define WES_MODE_NONE       0

GLenum          vt_mode;
GLuint          vt_count;
sVertexAttrib   vt_vbuffer[WES_BUFFER_COUNT];
GLuint          vt_vbuffer_count;
GLshort         vt_ibuffer[WES_INDEX_COUNT];
GLuint          vt_ibuffer_count;
sVertexAttrib   vt_current[1];
GLuint          vt_possize, vt_color0size, vt_color1size,
                vt_texcoord0size, vt_texcoord1size,
                vt_normalsize, vt_fogcoordsize;

struct sRenderState     vt_state[1];
struct sRenderState     vt_state_next[1];
GLboolean               vt_state_changed;

const sVertexAttrib initialconst = {
    0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0,
    0.0,
    0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0
    };

GLvoid
wes_reset()
{
    vt_mode = WES_MODE_NONE;
    vt_count = 0;
    vt_current[0] = initialconst;
}

//functions:
GLvoid
wes_vertbuffer_flush()
{
    if (!vt_count){
        return;
    }

    wes_gl->glUseProgram(sh_program);

    wes_state_update();

    wes_gl->glEnableVertexAttribArray(WES_APOS);
    wes_gl->glVertexAttribPointer(WES_APOS, vt_possize, GL_FLOAT, GL_FALSE, sizeof(sVertexAttrib), vt_vbuffer);
    if (vt_texcoord0size){
        wes_gl->glEnableVertexAttribArray(WES_ATEXCOORD0);
        wes_gl->glVertexAttribPointer(WES_ATEXCOORD0, vt_texcoord0size, GL_FLOAT, GL_FALSE, sizeof(sVertexAttrib),
            (GLfloat*)(vt_vbuffer) + WES_OFFSET_TEXCOORD0);
    } else {
        wes_gl->glDisableVertexAttribArray(WES_ATEXCOORD0);
    }
    if (vt_texcoord1size){
        wes_gl->glEnableVertexAttribArray(WES_ATEXCOORD1);
        wes_gl->glVertexAttribPointer(WES_ATEXCOORD1, vt_texcoord1size, GL_FLOAT, GL_FALSE, sizeof(sVertexAttrib),
            (GLfloat*)(vt_vbuffer) + WES_OFFSET_TEXCOORD1);
    } else {
        wes_gl->glDisableVertexAttribArray(WES_ATEXCOORD1);
    }
    if (vt_normalsize){
        wes_gl->glEnableVertexAttribArray(WES_ANORMAL);
        wes_gl->glVertexAttribPointer(WES_ANORMAL, vt_normalsize, GL_FLOAT, GL_FALSE, sizeof(sVertexAttrib),
            (GLfloat*)(vt_vbuffer) + WES_OFFSET_NORMAL);
    } else {
        wes_gl->glDisableVertexAttribArray(WES_ANORMAL);
    }
    if (vt_fogcoordsize){
        wes_gl->glEnableVertexAttribArray(WES_AFOGCOORD);
        wes_gl->glVertexAttribPointer(WES_AFOGCOORD, vt_fogcoordsize, GL_FLOAT, GL_FALSE, sizeof(sVertexAttrib),
            (GLfloat*)(vt_vbuffer) + WES_OFFSET_FOGCOORD);
    } else {
        wes_gl->glDisableVertexAttribArray(WES_AFOGCOORD);
    }
    if (vt_color0size){
        wes_gl->glEnableVertexAttribArray(WES_ACOLOR0);
        wes_gl->glVertexAttribPointer(WES_ACOLOR0, vt_color0size, GL_FLOAT, GL_FALSE, sizeof(sVertexAttrib),
            (GLfloat*)(vt_vbuffer) + WES_OFFSET_COLOR0);
    } else {
        wes_gl->glDisableVertexAttribArray(WES_ACOLOR0);
    }
    if (vt_color1size){
        wes_gl->glEnableVertexAttribArray(WES_ACOLOR1);
        wes_gl->glVertexAttribPointer(WES_ACOLOR1, vt_color1size, GL_FLOAT, GL_FALSE, sizeof(sVertexAttrib),
            (GLfloat*)(vt_vbuffer) + WES_OFFSET_COLOR1);
    } else {
        wes_gl->glDisableVertexAttribArray(WES_ACOLOR1);
    }

    if (vt_mode == GL_QUADS){
        int i;
        int num = vt_count / 4;
        for(i = 0; i < num; i += 1){
            vt_ibuffer[i*6 + 0] = i*4 + 0;
            vt_ibuffer[i*6 + 1] = i*4 + 1;
            vt_ibuffer[i*6 + 2] = i*4 + 2;
            vt_ibuffer[i*6 + 3] = i*4 + 2;
            vt_ibuffer[i*6 + 4] = i*4 + 3;
            vt_ibuffer[i*6 + 5] = i*4 + 0;
        }
        wes_gl->glDrawElements(GL_TRIANGLES, num * 6, GL_UNSIGNED_SHORT, vt_ibuffer);
    } else {
        wes_gl->glDrawArrays(vt_mode, 0, vt_count);
    }
    wes_reset();
}

GLvoid
glBegin(GLenum mode)
{
    vt_mode = mode;

    vt_possize = vt_normalsize = vt_fogcoordsize = 0;
    vt_color1size = vt_color0size = vt_texcoord0size = vt_texcoord1size = 0;

    /* Hacks */
    if (vt_mode == GL_QUAD_STRIP){
        vt_mode = GL_TRIANGLE_STRIP;
    }
    if (vt_mode == GL_POLYGON){
        vt_mode = GL_TRIANGLE_FAN;
    }

    /* Set Constant data */
    wes_gl->glVertexAttrib4f(WES_APOS, vt_current->x, vt_current->y, vt_current->z, vt_current->w);
    wes_gl->glVertexAttrib4f(WES_ATEXCOORD0, vt_current->s0, vt_current->t0, vt_current->r0, vt_current->q0);
    wes_gl->glVertexAttrib4f(WES_ATEXCOORD1, vt_current->s1, vt_current->t1, vt_current->r1, vt_current->q1);
    wes_gl->glVertexAttrib3f(WES_ANORMAL, vt_current->nx, vt_current->ny, vt_current->nz);
    wes_gl->glVertexAttrib1f(WES_AFOGCOORD, vt_current->fog);
    wes_gl->glVertexAttrib4f(WES_ACOLOR0, vt_current->cr0, vt_current->cg0, vt_current->cb0, vt_current->ca0);
    wes_gl->glVertexAttrib3f(WES_ACOLOR1, vt_current->cr1, vt_current->cg1, vt_current->cb1);
}

//Vertex specification:
//glVertex
GLvoid
glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    vt_possize = 4;
    vt_current->x = x;
    vt_current->y = y;
    vt_current->z = z;
    vt_current->w = w;
    vt_vbuffer[vt_count++] = vt_current[0];
}

GLvoid
glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    if (vt_possize > 3){
        glVertex4f(x, y, z, 1.0);
    } else {
        vt_possize = 3;
        vt_current->x = x;
        vt_current->y = y;
        vt_current->z = z;
        vt_vbuffer[vt_count++] = vt_current[0];
    }
}

GLvoid
glVertex2f(GLfloat x, GLfloat y)
{
    if (vt_possize > 2) {
        glVertex3f(x, y, 0.0);
    } else {
        vt_possize = 2;
        vt_current->x = x;
        vt_current->y = y;
        vt_vbuffer[vt_count++] = vt_current[0];
    }
}

//glTexCoord
GLvoid
glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    vt_texcoord0size = 4;
    vt_current->s0 = s;
    vt_current->t0 = t;
    vt_current->r0 = r;
    vt_current->q0 = q;
}

GLvoid
glTexCoord3f(GLfloat s, GLfloat t, GLfloat r)
{
    if (vt_texcoord0size > 3){
        glTexCoord4f(s, t, r, 0);
    } else {
        vt_texcoord0size = 3;
        vt_current->s0 = s;
        vt_current->t0 = t;
        vt_current->r0 = r;
    }
}

GLvoid
glTexCoord2f(GLfloat s, GLfloat t)
{
    if (vt_texcoord0size > 2){
        glTexCoord3f(s, t, 0);
    } else {
        vt_texcoord0size = 2;
        vt_current->s0 = s;
        vt_current->t0 = t;
    }
}

GLvoid
glTexCoord1f(GLfloat s)
{
    if (vt_texcoord0size > 1){
        glTexCoord2f(s, 0);
    } else {
        vt_texcoord0size = 1;
        vt_current->s0 = s;
    }
}


//glMultiTexCoord
GLvoid
glMultiTexCoord4f(GLenum tex, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    if (tex == GL_TEXTURE0){
        glTexCoord4f(s, t, r, q);
    } else {
        vt_texcoord1size = 4;
        vt_current->s1 = s;
        vt_current->t1 = t;
        vt_current->r1 = r;
        vt_current->q1 = q;
    }
}

//glNormal
GLvoid
glNormal3f(GLfloat x, GLfloat y, GLfloat z)
{
    vt_normalsize = 3;
    vt_current->nx = x;
    vt_current->ny = y;
    vt_current->nz = z;
}

//glFogCoord
GLvoid
glFogCoordf(GLfloat f)
{
    vt_fogcoordsize = 1;
    vt_current->fog = f;
}

//glColor
GLvoid
glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    vt_color0size = 4;
    vt_current->cr0 = r;
    vt_current->cg0 = g;
    vt_current->cb0 = b;
    vt_current->ca0 = a;
}

GLvoid
glColor3f(GLfloat r, GLfloat g, GLfloat b)
{
    if (vt_color0size > 3){
        glColor4f(r, g, b, 1);
    } else {
        vt_color0size = 3;
        vt_current->cr0 = r;
        vt_current->cg0 = g;
        vt_current->cb0 = b;
    }
}

//glSecondaryColor
GLvoid
glSecondaryColor3f(GLfloat r, GLfloat g, GLfloat b){
    vt_color1size = 3;
    vt_current->cr1 = r;
    vt_current->cg1 = g;
    vt_current->cb1 = b;
}

GLvoid
glEnd()
{
    wes_vertbuffer_flush();
}

GLvoid
wes_begin_init()
{
    wes_reset();
}

GLvoid
wes_begin_destroy()
{
    //free(vt_vbuffer);
    //free(vt_ibuffer);
}
