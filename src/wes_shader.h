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
