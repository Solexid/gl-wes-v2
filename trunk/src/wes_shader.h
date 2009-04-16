#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wes.h"
#include "wes_uniform.h"

#ifndef __WES_SHADER_H__
#define __WES_SHADER_H__

//attribute indices:
#define WES_APOS             0
#define WES_ATEXCOORD0       1
#define WES_ATEXCOORD1       2
#define WES_ANORMAL          3
#define WES_AFOGCOORD        4
#define WES_ACOLOR0          5
#define WES_ACOLOR1          6
#define WES_ANUM             7

extern GLuint       sh_program;
extern GLuint       sh_fragment;
extern GLuint       sh_vertex;

extern GLvoid       wes_shader_init();
extern GLvoid       wes_shader_destroy();

#endif
