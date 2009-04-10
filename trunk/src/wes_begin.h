
#include "wes_gl_defines.h"

#ifndef __WES_BEGIN_H__
#define __WES_BEGIN_H__


/* Initial Buffer sizes*/
#define WES_BUFFER_COUNT        (8000)
#define WES_INDEX_COUNT         (8000)

//offsets within VertexAttrib struct
#define WES_OFFSET_VERT        0
#define WES_OFFSET_TEXCOORD0   4
#define WES_OFFSET_TEXCOORD1   8
#define WES_OFFSET_NORMAL      12
#define WES_OFFSET_FOGCOORD    15
#define WES_OFFSET_COLOR0      16
#define WES_OFFSET_COLOR1      20

//Structures:
typedef struct sVertexAttrib sVertexAttrib;
struct sVertexAttrib {
    GLfloat x, y, z, w;             //Position , 2-4 elements
    GLfloat s0, t0, r0, q0;         //TexCoord0, 2-4 elements
    GLfloat s1, t1, r1, q1;         //TexCoord1, 2-4 elements (for 2x multitexturing support)
    GLfloat nx, ny, nz;             //Normal, 3 elements
    GLfloat fog;                    //Fog Coordinate, 1 element.
    GLfloat cr0, cg0, cb0, ca0;     //Color, 3-4 elements
    GLfloat cr1, cg1, cb1;          //Secondary Color, 3 elements
};

typedef struct sRenderState sRenderState;
struct sRenderState {

};


//function declarations:
extern GLvoid wes_begin_init();
extern GLvoid wes_begin_destroy();

#endif
