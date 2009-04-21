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

#include "wes_gl_defines.h"

#ifndef __WES_BEGIN_H__
#define __WES_BEGIN_H__


/* Initial Buffer sizes*/
#define WES_BUFFER_COUNT        (8000)
#define WES_INDEX_COUNT         (8000)

//offsets within Vertex struct
#define WES_OFFSET_VERT        0
#define WES_OFFSET_NORMAL      4
#define WES_OFFSET_FOGCOORD    7
#define WES_OFFSET_COLOR0      8
#define WES_OFFSET_COLOR1      12
#define WES_OFFSET_TEXCOORD0   15
#define WES_OFFSET_TEXCOORD1   19
#define WES_OFFSET_TEXCOORD2   23
#define WES_OFFSET_TEXCOORD3   27

typedef struct vertex_s vertex_t;

struct vertex_s {
    GLfloat x, y, z, w;             //Position , 2-4 elements
    GLfloat nx, ny, nz;             //Normal, 3 elements
    GLfloat fog;                    //Fog Coordinate, 1 element.
    GLfloat cr0, cg0, cb0, ca0;     //Color, 3-4 elements
    GLfloat cr1, cg1, cb1;          //Secondary Color, 3 elements
    struct {
        GLfloat s, t, r, q;
    } coord[4];
};

//function declarations:
extern GLvoid wes_begin_init();
extern GLvoid wes_begin_destroy();

#endif
