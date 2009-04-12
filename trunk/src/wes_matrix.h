
#include "wes_gl_defines.h"

#ifndef __WES_MATRIX_H__
#define __WES_MATRIX_H__

/* number of matrices for push/pop      */
#define WES_MODELVIEW_NUM           32
#define WES_PROJECTION_NUM          2
#define WES_TEXTURE_NUM             2
#define WES_TEXTURE_UNITS           8
#define WES_COLOR_NUM               2

/* flags for matrix operations          */
#define WES_M_IDENTITY              0x01
#define WES_M_SCALED                0x02
#define WES_M_ROTATED               0x04
#define WES_M_TRANSLATED            0x08
#define WES_M_DIRTY                 0x10

/* matrix structure                     */
typedef struct matrix4_s matrix4_t;
struct matrix4_s {
    GLuint  flags;
    GLfloat data[16];
};

/* global variables                     */
extern GLenum      m_mode;
extern GLenum      m_activetexture;
extern matrix4_t*  m_current;
extern matrix4_t   m_modelview[WES_MODELVIEW_NUM];
extern matrix4_t   m_projection[WES_PROJECTION_NUM];
extern matrix4_t   m_texture[WES_TEXTURE_NUM * WES_TEXTURE_UNITS];
extern matrix4_t   m_color[WES_COLOR_NUM];
extern matrix4_t   m_modelview_proj[1];
extern matrix4_t   m_modelview_it[1];
extern GLfloat     m_modelview_it3[9];
extern GLuint      m_modelview_num, m_projection_num, m_texture_num, m_color_num;
extern GLboolean   m_modelview_mod;
extern GLboolean   m_projection_mod;


extern GLvoid       wes_matrix_init();
extern GLvoid       wes_matrix_destroy();
extern GLboolean    wes_matrix_mvp();
extern GLboolean    wes_matrix_mvit();
#endif
