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

#ifndef __WES_SHADER_H__
#define __WES_SHADER_H__

#define WES_APOS             0
#define WES_ANORMAL          1
#define WES_AFOGCOORD        2
#define WES_ACOLOR0          3
#define WES_ACOLOR1          4
#define WES_ATEXCOORD0       5
#define WES_ATEXCOORD1       6
#define WES_ATEXCOORD2       7
#define WES_ATEXCOORD3       8
#define WES_ANUM             9

extern GLuint       sh_program;
extern GLuint       sh_fragment;
extern GLuint       sh_vertex;

extern GLvoid       wes_shader_init();
extern GLvoid       wes_shader_destroy();

#endif
