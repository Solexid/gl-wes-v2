
#include <math.h>
#include <string.h>
#include "wes.h"
#include "wes_matrix.h"

/* global variables */
GLenum      m_mode;
GLenum      m_activetexture;

matrix4_t*  m_current;
matrix4_t   m_modelview[WES_MODELVIEW_NUM];
matrix4_t   m_projection[WES_PROJECTION_NUM];
matrix4_t   m_texture[WES_TEXTURE_NUM * WES_TEXTURE_UNITS];
matrix4_t   m_color[WES_COLOR_NUM];
matrix4_t   m_modelview_proj[1];
matrix4_t   m_modelview_it[1];
GLfloat     m_modelview_it3[9];

GLuint      m_modelview_num, m_projection_num, m_texture_num, m_color_num;
GLboolean   m_modelview_mod;
GLboolean   m_projection_mod;


/* internal functions   */
GLvoid
wes_assign(GLfloat *in, matrix4_t *out)
{
    memcpy(out->data, in, 16 * sizeof(GLfloat));
    out->flags = WES_M_DIRTY;
};

GLvoid
wes_classify(matrix4_t *in)
{
    in->flags = 0;

    if (in->data[3] != 0 || in->data[7] != 0 || in->data[11] != 0 || in->data[15] != 1.0){
        in->flags = WES_M_DIRTY;
        return;
    }

    if (in->data[1] == 0 && in->data[2] == 0 && in->data[4] == 0 &&
        in->data[6] == 0 && in->data[8] == 0 && in->data[9] == 0){

        if (in->data[0] == 1.0 && in->data[5] == 1.0 && in->data[10] == 1.0){
            in->flags = WES_M_IDENTITY;
        } else {
            in->flags = WES_M_SCALED;
        }

    } else {
        in->flags = WES_M_ROTATED;
    }

    if (in->data[12] != 0 || in->data[13] != 0 || in->data[14] == 0){
        in->flags |= WES_M_TRANSLATED;
    }
}

GLvoid
wes_transpose2(GLfloat *in, GLfloat *out)
{
    out[0] = in[0]; out[1] = in[2];
    out[2] = in[1]; out[3] = in[3];
}

GLvoid
wes_transpose3(GLfloat *in, GLfloat *out)
{
    out[0] = in[0]; out[1] = in[3]; out[2] = in[6];
    out[3] = in[1]; out[4] = in[4]; out[5] = in[7];
    out[6] = in[2]; out[7] = in[5]; out[8] = in[8];
}

GLvoid
wes_transpose4(GLfloat *in, GLfloat *out)
{
    out[0] = in[0]; out[1] = in[4]; out[2] = in[8]; out[3] = in[12];
    out[4] = in[1]; out[5] = in[5]; out[6] = in[9]; out[7] = in[13];
    out[8] = in[2]; out[9] = in[6]; out[10] = in[10]; out[11] = in[14];
    out[12] = in[3]; out[13] = in[7]; out[14] = in[11]; out[15] = in[15];
}

GLvoid
wes_identity4(matrix4_t *m)
{
    m->flags = WES_M_IDENTITY;
    m->data[1] = m->data[2] = m->data[3] = m->data[4] = 0.0;
    m->data[6] = m->data[7] = m->data[8] = m->data[9] = 0.0;
    m->data[11] = m->data[12] = m->data[13] = m->data[14] = 0.0;
    m->data[0] = m->data[5] = m->data[10] = m->data[15] = 1.0;
}

GLvoid
wes_mat4to3(GLfloat *in, GLfloat *out )
{
    out[0] = in[0]; out[3] = in[4]; out[6] = in[8];
    out[1] = in[1]; out[4] = in[5]; out[7] = in[9];
    out[2] = in[2]; out[5] = in[6]; out[8] = in[10];
}

GLvoid
wes_mul4(matrix4_t *in0, matrix4_t *in1, matrix4_t *out)
{
    if (in0->flags == WES_M_IDENTITY){
        *out = *in1;
    } else if (in1->flags == WES_M_IDENTITY){
        *out = *in0;
    } else {
        wes_mul4_general(in0->data, in1->data, out->data);
    }
}

GLvoid
normalize3(GLfloat *v)
{
    GLfloat r;
    r = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (r == 0.0) return;
    v[0] /= r;
    v[1] /= r;
    v[2] /= r;
}

GLvoid
cross3(GLfloat *v1, GLfloat *v2, GLfloat *result)
{
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

GLvoid
wes_mul4_general(GLfloat *in0, GLfloat *in1, GLfloat *out)
{
    out[0]  = in0[0] * in1[0] + in0[1] * in1[4] + in0[2] * in1[8] + in0[3] * in1[12];
	out[1]  = in0[0] * in1[1] + in0[1] * in1[5] + in0[2] * in1[9] + in0[3] * in1[13];
	out[2]  = in0[0] * in1[2] + in0[1] * in1[6] + in0[2] * in1[10] + in0[3] * in1[14];
	out[3]  = in0[0] * in1[3] + in0[1] * in1[7] + in0[2] * in1[11] + in0[3] * in1[15];
	out[4]  = in0[4] * in1[0] + in0[5] * in1[4] + in0[6] * in1[8] + in0[7] * in1[12];
	out[5]  = in0[4] * in1[1] + in0[5] * in1[5] + in0[6] * in1[9] + in0[7] * in1[13];
	out[6]  = in0[4] * in1[2] + in0[5] * in1[6] + in0[6] * in1[10] + in0[7] * in1[14];
	out[7]  = in0[4] * in1[3] + in0[5] * in1[7] + in0[6] * in1[11] + in0[7] * in1[15];
	out[8]  = in0[8] * in1[0] + in0[9] * in1[4] + in0[10] * in1[8] + in0[11] * in1[12];
	out[9]  = in0[8] * in1[1] + in0[9] * in1[5] + in0[10] * in1[9] + in0[11] * in1[13];
	out[10] = in0[8] * in1[2] + in0[9] * in1[6] + in0[10] * in1[10] + in0[11] * in1[14];
	out[11] = in0[8] * in1[3] + in0[9] * in1[7] + in0[10] * in1[11] + in0[11] * in1[15];
	out[12] = in0[12] * in1[0] + in0[13] * in1[4] + in0[14] * in1[8] + in0[15] * in1[12];
	out[13] = in0[12] * in1[1] + in0[13] * in1[5] + in0[14] * in1[9] + in0[15] * in1[13];
	out[14] = in0[12] * in1[2] + in0[13] * in1[6] + in0[14] * in1[10] + in0[15] * in1[14];
	out[15] = in0[12] * in1[3] + in0[13] * in1[7] + in0[14] * in1[11] + in0[15] * in1[15];
}

GLvoid
wes_rotate_xyz(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat mag;
    GLfloat xx, yy, zz, xy, yz, zx, xs,ys,zs;
    GLfloat s, c, one_c;
    GLfloat m0, m1, m2, m4, m5, m6, m8, m9, m10;
    GLfloat tmp[12];

    angle *= M_PI / 180;
    s = sin(angle);
    c = cos(angle);

    mag = sqrt(x * x + y * y + z * z);
    x /= mag;
    y /= mag;
    z /= mag;

    xx = x * x; yy = y * y; zz = z * z;
    xy = x * y; yz = y * z; zx = z * x;
    xs = x * s; ys = y * s; zs = z * s;
    one_c = 1.0 - c;

    m0 = (one_c * xx) + c;
    m1 = (one_c * xy) + zs;
    m2 = (one_c * zx) - ys;
    m4 = (one_c * xy) - zs;
    m5 = (one_c * yy) + c;
    m6 = (one_c * yz) + xs;
    m8 = (one_c * zx) + ys;
    m9 = (one_c * yz) - xs;
    m10 = (one_c * zz) + c;

    if (m_current->flags == WES_M_IDENTITY || m_current->flags == (WES_M_IDENTITY | WES_M_TRANSLATED)){
        m_current->data[0]  = m0;
        m_current->data[1]  = m1;
        m_current->data[2]  = m2;
        m_current->data[4]  = m4;
        m_current->data[5]  = m5;
        m_current->data[6]  = m6;
        m_current->data[8]  = m8;
        m_current->data[9]  = m9;
        m_current->data[10] = m10;
    } else if (m_current->flags == WES_M_SCALED || m_current->flags == (WES_M_SCALED | WES_M_TRANSLATED)){
        tmp[0] = m0 * m_current->data[0];
        tmp[1] = m1 * m_current->data[5];
        tmp[2] = m2 * m_current->data[10];
        tmp[3] = m4 * m_current->data[0];
        tmp[4] = m5 * m_current->data[5];
        tmp[5] = m6 * m_current->data[10];
        tmp[6] = m8 * m_current->data[0];
        tmp[7] = m9 * m_current->data[5];
        tmp[8] = m10 * m_current->data[10];
        m_current->data[0]  = tmp[0];
        m_current->data[1]  = tmp[1];
        m_current->data[2]  = tmp[2];
        m_current->data[4]  = tmp[3];
        m_current->data[5]  = tmp[4];
        m_current->data[6]  = tmp[5];
        m_current->data[8]  = tmp[6];
        m_current->data[9]  = tmp[7];
        m_current->data[10] = tmp[8];
    } else {
        tmp[0]  = m0 * m_current->data[0] + m1 * m_current->data[4] + m2 * m_current->data[8];
        tmp[1]  = m0 * m_current->data[1] + m1 * m_current->data[5] + m2 * m_current->data[9];
        tmp[2]  = m0 * m_current->data[2] + m1 * m_current->data[6] + m2 * m_current->data[10];
        tmp[3]  = m0 * m_current->data[3] + m1 * m_current->data[7] + m2 * m_current->data[11];
        tmp[4]  = m4 * m_current->data[0] + m5 * m_current->data[4] + m6 * m_current->data[8];
        tmp[5]  = m4 * m_current->data[1] + m5 * m_current->data[5] + m6 * m_current->data[9];
        tmp[6]  = m4 * m_current->data[2] + m5 * m_current->data[6] + m6 * m_current->data[10];
        tmp[7]  = m4 * m_current->data[3] + m5 * m_current->data[7] + m6 * m_current->data[11];
        tmp[8]  = m8 * m_current->data[0] + m9 * m_current->data[4] + m10 * m_current->data[8];
        tmp[9]  = m8 * m_current->data[1] + m9 * m_current->data[5] + m10 * m_current->data[9];
        tmp[10] = m8 * m_current->data[2] + m9 * m_current->data[6] + m10 * m_current->data[10];
        tmp[11] = m8 * m_current->data[3] + m9 * m_current->data[7] + m10 * m_current->data[11];
        memcpy(m_current->data, tmp, 12 * sizeof(GLfloat));
    }


    if (!(m_current->flags & WES_M_DIRTY)){
        if (m_current->flags & WES_M_TRANSLATED){
            m_current->flags = WES_M_ROTATED | WES_M_TRANSLATED;
        } else {
            m_current->flags = WES_M_ROTATED;
        }
    }
}

GLvoid
wes_rotate_z(GLfloat angle, GLfloat z)
{
    GLfloat s, c;
    GLfloat m1, m4;
    GLfloat tmp[8];

    angle *= (M_PI / 180);
    s = sin(angle);
    c = cos(angle);

    if (z < 0.0f) {
       m1 = -s;
       m4 = s;
    } else {
       m1 = s;
       m4 = -s;
    }

    if (m_current->flags == WES_M_IDENTITY || m_current->flags == (WES_M_IDENTITY | WES_M_TRANSLATED)){
        m_current->data[0]  = c;
        m_current->data[1]  = m1;
        m_current->data[4]  = m4;
        m_current->data[5]  = c;
    } else if (m_current->flags == WES_M_SCALED || m_current->flags == (WES_M_SCALED | WES_M_TRANSLATED)){
        tmp[0] = c * m_current->data[0];
        tmp[1] = m1 * m_current->data[5];
        tmp[2] = m4 * m_current->data[0];
        tmp[3] = c * m_current->data[5];
        m_current->data[0] = tmp[0];
        m_current->data[1] = tmp[1];
        m_current->data[4] = tmp[2];
        m_current->data[5] = tmp[3];
    } else {
        tmp[0] = c * m_current->data[0] + m1 * m_current->data[4];
        tmp[1] = c * m_current->data[1] + m1 * m_current->data[5];
        tmp[2] = c * m_current->data[2] + m1 * m_current->data[6];
        tmp[3] = c * m_current->data[3] + m1 * m_current->data[7];
        tmp[4] = m4 * m_current->data[0] + c * m_current->data[4];
        tmp[5] = m4 * m_current->data[1] + c * m_current->data[5];
        tmp[6] = m4 * m_current->data[2] + c * m_current->data[6];
        tmp[7] = m4 * m_current->data[3] + c * m_current->data[7];
        memcpy(m_current->data, tmp, 8 * sizeof(GLfloat));
    }
    m_current->flags |= WES_M_ROTATED;
}

GLvoid
wes_rotate_y(GLfloat angle, GLfloat y)
{
    GLfloat s, c;
    GLfloat m2, m8;
    GLfloat tmp[8];

    angle *= M_PI / 180;
    s = sin(angle);
    c = cos(angle);

    if (y < 0.0f) {
        m2 = s;
        m8 = -s;
    } else {
        m2 = -s;
        m8 = s;
    }

    if (m_current->flags == WES_M_IDENTITY || m_current->flags == (WES_M_IDENTITY | WES_M_TRANSLATED)){
        m_current->data[0] = c;
        m_current->data[2] = m2;
        m_current->data[12] = m8;
        m_current->data[14] = c;
    } else if (m_current->flags == WES_M_SCALED || m_current->flags == (WES_M_SCALED | WES_M_TRANSLATED)){
        tmp[0] = c * m_current->data[0];
        tmp[1] = m2 * m_current->data[10];
        tmp[2] = m8 * m_current->data[0];
        tmp[3] = c * m_current->data[10];
        m_current->data[0] = tmp[0];
        m_current->data[2] = tmp[1];
        m_current->data[12] = tmp[2];
        m_current->data[14] = tmp[3];
    } else {
        tmp[0]  = c * m_current->data[0] + m2 * m_current->data[8];
        tmp[1]  = c * m_current->data[1] + m2 * m_current->data[9];
        tmp[2]  = c * m_current->data[2] + m2 * m_current->data[10];
        tmp[3]  = c * m_current->data[3] + m2 * m_current->data[11];
        tmp[4]  = m8 * m_current->data[0] + c * m_current->data[8];
        tmp[5]  = m8 * m_current->data[1] + c * m_current->data[9];
        tmp[6] = m8 * m_current->data[2] + c * m_current->data[10];
        tmp[7] = m8 * m_current->data[3] + c * m_current->data[11];
        memcpy(m_current->data, tmp, 4 * sizeof(GLfloat));
        memcpy(m_current->data + 8, tmp + 4, 4 * sizeof(GLfloat));
    }

    m_current->flags |= WES_M_ROTATED;
}

GLvoid
wes_rotate_x(GLfloat angle, GLfloat x)
{
    GLfloat s, c;
    GLfloat m6, m9;
    GLfloat tmp[8];

    angle *= M_PI / 180;
    s = sin(angle);
    c = cos(angle);

    if (x < 0.0f) {
        m9 = s;
        m6 = -s;
    } else {
        m9 = -s;
        m6 = s;
    }

    if (m_current->flags == WES_M_IDENTITY || m_current->flags == (WES_M_IDENTITY | WES_M_TRANSLATED)){
        m_current->data[5] = c;
        m_current->data[6] = m6;
        m_current->data[9] = m9;
        m_current->data[10] = c;
    } else if (m_current->flags == WES_M_SCALED || m_current->flags == (WES_M_SCALED | WES_M_TRANSLATED)){
        tmp[0] = c * m_current->data[5];
        tmp[1] = m6 * m_current->data[10];
        tmp[2] = m9 * m_current->data[5];
        tmp[3] = c * m_current->data[10];
        m_current->data[5] = tmp[0];
        m_current->data[6] = tmp[1];
        m_current->data[9] = tmp[2];
        m_current->data[10] = tmp[3];
    } else {
        tmp[0] = c * m_current->data[4] + m6 * m_current->data[8];
        tmp[1] = c * m_current->data[5] + m6 * m_current->data[9];
        tmp[2] = c * m_current->data[6] + m6 * m_current->data[10];
        tmp[3] = c * m_current->data[7] + m6 * m_current->data[11];
        tmp[4] = m9 * m_current->data[4] + c * m_current->data[8];
        tmp[5] = m9 * m_current->data[5] + c * m_current->data[9];
        tmp[6] = m9 * m_current->data[6] + c * m_current->data[10];
        tmp[7] = m9 * m_current->data[7] + c * m_current->data[11];
        memcpy(m_current->data + 4, tmp, 8 * sizeof(GLfloat));
    }

    m_current->flags |= WES_M_ROTATED;
}

GLvoid
wes_matrix_init()
{
    //Set all matrices to identity:
    int i;
    for(i = 0; i < WES_MODELVIEW_NUM; i++){
        wes_identity(&m_modelview[i]);
    }

    for(i = 0; i < WES_PROJECTION_NUM; i++){
        wes_identity(&m_projection[i]);
    }
    for(i = 0; i < WES_TEXTURE_NUM * WES_TEXTURE_UNITS; i++){
        wes_identity(&m_texture[i]);
    }

    for(i = 0; i < WES_COLOR_NUM; i++){
        wes_identity(&m_color[i]);
    }

    m_modelview_num = m_projection_num = 1;
    m_texture_num = m_color_num = 1;
    m_current = m_modelview;
    m_mode = GL_MODELVIEW;
    m_modelview_mod = m_projection_mod = 0;
}

GLvoid
wes_invert_scaled(matrix4_t *in, matrix4_t *out)
{
    if (in->data[0] == 0 || in->data[5] || in->data[10] == 0){
        return;
    }
    out->flags = WES_M_SCALED;
    out->data[0] = (1.0 / in->data[0]);
    out->data[1] = out->data[2] = out->data[3] = out->data[4] = 0.0;
    out->data[5] = (1.0 / in->data[5]);
    out->data[6] = out->data[7] = out->data[8] = out->data[9] = 0.0;
    out->data[10] = (1.0 / in->data[10]);
    out->data[11] = 0.0;
    if (in->flags & WES_M_TRANSLATED){
        out->flags |= WES_M_TRANSLATED;
        out->data[12] = - (in->data[12] * out->data[0] + in->data[13]);
        out->data[13] = - (in->data[13] * out->data[5]);
        out->data[14] = - (in->data[13] * in->data[14] * out->data[10]);
    } else {
        out->data[12] = out->data[13] = out->data[14] = 0.0;
    }
    out->data[15] = 1.0;
}

GLvoid
wes_invert_rotated(matrix4_t *in, matrix4_t *out)
{
    GLfloat det;

    det = 0;
    det += in->data[0] * in->data[5] * in->data[10];
    det += in->data[1] * in->data[6] * in->data[8];
    det += in->data[2] * in->data[4] * in->data[9];
    det -= in->data[2] * in->data[5] * in->data[8];
    det -= in->data[1] * in->data[4] * in->data[10];
    det -= in->data[0] * in->data[6] * in->data[9];

    if (det == 0){
      return;
    }

    det = 1.0f / det;
    out->flags = WES_M_ROTATED;
    out->data[0] = + det * (in->data[5] * in->data[10] - in->data[6] * in->data[9]);
    out->data[1] = - det * (in->data[1] * in->data[10] + in->data[2] * in->data[9]);
    out->data[2] = + det * (in->data[1] * in->data[6] - in->data[2] * in->data[5]);
    out->data[3] = 0.0;
    out->data[4] = - det * (in->data[4] * in->data[10] + in->data[6] * in->data[8]);
    out->data[5] = + det * (in->data[0] * in->data[10] - in->data[2] * in->data[8]);
    out->data[6] = - det * (in->data[0] * in->data[6] - in->data[2] * in->data[4]);
    out->data[7] = 0.0;
    out->data[8] = + det * (in->data[4] * in->data[9] - in->data[5] * in->data[8]);
    out->data[9] = - det * (in->data[0] * in->data[9] + in->data[1] * in->data[8]);
    out->data[10] = + det * (in->data[0] * in->data[5] - in->data[1] * in->data[4]);
    out->data[11] = 0.0;
    if (in->flags & WES_M_TRANSLATED){
        out->flags |= WES_M_TRANSLATED;
        out->data[12] = - ((in->data[12] * out->data[0] + in->data[13]) * (out->data[4] + in->data[14] * out->data[8]));
        out->data[13] = - ((in->data[12] * out->data[1] + in->data[13]) * (out->data[5] + in->data[14] * out->data[9]));
        out->data[14] = - ((in->data[12] * out->data[2] + in->data[13]) * (out->data[6] + in->data[14] * out->data[10]));
    } else {
        out->data[12] = out->data[13] = out->data[14] = 0.0;
    }
    out->data[15] = 1.0;
}

GLvoid
wes_invert_general(matrix4_t *in, matrix4_t *out)
{
    GLfloat det, tmp[12];

    tmp[0] = in->data[0] * in->data[5] - in->data[1] * in->data[4];
    tmp[1] = in->data[0] * in->data[6] - in->data[2] * in->data[4];
    tmp[2] = in->data[0] * in->data[7] - in->data[3] * in->data[4];
    tmp[3] = in->data[1] * in->data[6] - in->data[2] * in->data[5];
    tmp[4] = in->data[1] * in->data[7] - in->data[3] * in->data[5];
    tmp[5] = in->data[2] * in->data[7] - in->data[3] * in->data[6];
    tmp[6] = in->data[8] * in->data[13] - in->data[9] * in->data[12];
    tmp[7] = in->data[8] * in->data[14] - in->data[10] * in->data[12];
    tmp[8] = in->data[8] * in->data[15] - in->data[11] * in->data[12];
    tmp[9] = in->data[9] * in->data[14] - in->data[10] * in->data[13];
    tmp[10] = in->data[9] * in->data[15] - in->data[11] * in->data[13];
    tmp[11] = in->data[10] * in->data[15] - in->data[11] * in->data[14];

    det = tmp[0] * tmp[11] - tmp[1] * tmp[10] + tmp[2] * tmp[9] + tmp[3] * tmp[8] - tmp[4] * tmp[7] + tmp[5] * tmp[6];
    if (det == 0){
        return;
    }

    det = 1.0 / det;
    out->data[0]  = + det * (in->data[5] * tmp[11]  - in->data[6] * tmp[10]  + in->data[7] * tmp[9]);
    out->data[1]  = - det * (in->data[1] * tmp[11]  + in->data[2] * tmp[10]  - in->data[3] * tmp[9]);
    out->data[2]  = + det * (in->data[13] * tmp[5] - in->data[14] * tmp[4] + in->data[15] * tmp[3]);
    out->data[3]  = - det * (in->data[9] * tmp[5]  + in->data[10] * tmp[4] - in->data[11] * tmp[3]);
    out->data[4]  = - det * (in->data[4] * tmp[11]  + in->data[6] * tmp[8]  - in->data[7] * tmp[7]);
    out->data[5]  = + det * (in->data[0] * tmp[11]  - in->data[2] * tmp[8]  + in->data[3] * tmp[7]);
    out->data[6]  = - det * (in->data[12] * tmp[5] + in->data[14] * tmp[2] - in->data[15] * tmp[1]);
    out->data[7]  = + det * (in->data[8] * tmp[5]  - in->data[10] * tmp[2] + in->data[11] * tmp[1]);
    out->data[8]  = + det * (in->data[4] * tmp[10]  - in->data[5] * tmp[8]  + in->data[7] * tmp[6]);
    out->data[9]  = - det * (in->data[0] * tmp[10]  + in->data[1] * tmp[8]  - in->data[3] * tmp[6]);
    out->data[10] = + det * (in->data[12] * tmp[4] - in->data[13] * tmp[2] + in->data[15] * tmp[0]);
    out->data[11] = - det * (in->data[8] * tmp[4]  + in->data[9] * tmp[2]  - in->data[11] * tmp[0]);
    out->data[12] = - det * (in->data[4] * tmp[9]  + in->data[5] * tmp[7]  - in->data[6] * tmp[6]);
    out->data[13] = + det * (in->data[0] * tmp[9]  - in->data[1] * tmp[7]  + in->data[2] * tmp[6]);
    out->data[14] = - det * (in->data[12] * tmp[3] + in->data[13] * tmp[1] - in->data[14] * tmp[0]);
    out->data[15] = + det * (in->data[8] * tmp[3]  - in->data[9] * tmp[1]  + in->data[10] * tmp[0]);
}

GLvoid
wes_invert(matrix4_t *in, matrix4_t *out)
{
    if (in->flags == WES_M_IDENTITY){
        *out = *in;
    } else if (in->flags == WES_M_SCALED || in->flags == (WES_M_SCALED|WES_M_TRANSLATED)){
        wes_invert_scaled(in, out);
    } else if (in->flags == WES_M_ROTATED || in->flags == (WES_M_ROTATED|WES_M_TRANSLATED)){
        wes_invert_rotated(in, out);
    } else {
        wes_invert_general(in, out);
    }
}

GLboolean
wes_matrix_mvit()
{
    if (m_modelview_mod){
        wes_invert(m_modelview, m_modelview_it);
        wes_mat4to3(m_modelview_it->data, m_modelview_it3);
        return 1;
    } else {
        return 0;
    }
}

GLboolean
wes_matrix_mvp()
{
    if (m_modelview_mod || m_projection_mod){
        wes_mul4(m_modelview, m_projection, m_modelview_proj);
        m_modelview_mod = m_projection_mod = 0;
        return 1;
    } else {
        return 0;
    }
}


/* opengl api   */
GLvoid
glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, GLfloat* value)
{
    if (transpose){
        GLfloat tmp[4];
        wes_transpose2(value, tmp);
        wes_gl->glUniformMatrix2fv(location, count, GL_FALSE, tmp);
    } else {
        wes_gl->glUniformMatrix2fv(location, count, GL_FALSE, value);
    }
}

GLvoid
glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, GLfloat* value)
{
    if (transpose){
        GLfloat tmp[9];
        wes_transpose3(value, tmp);
        wes_gl->glUniformMatrix3fv(location, count, GL_FALSE, tmp);
    } else {
        wes_gl->glUniformMatrix3fv(location, count, GL_FALSE, value);
    }
}

GLvoid
glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, GLfloat* value)
{
    if (transpose){
        GLfloat tmp[16];
        wes_transpose4(value, tmp);
        wes_gl->glUniformMatrix4fv(location, count, GL_FALSE, tmp);
    } else {
        wes_gl->glUniformMatrix4fv(location, count, GL_FALSE, value);
    }
}

GLvoid
glMatrixMode(GLenum mode)
{
    m_mode = mode;
    switch(m_mode)
    {
        case GL_MODELVIEW:  m_current = m_modelview; break;
        case GL_PROJECTION: m_current = m_projection; break;
        case GL_TEXTURE:    m_current = &m_texture[m_activetexture * WES_TEXTURE_NUM]; break;
        case GL_COLOR:      m_current = m_color; break;
        default: break;
    }
}

GLvoid
glLoadMatrixf(GLfloat *m)
{
    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    wes_assign(m, m_current);
    wes_classify(m_current);
}

GLvoid
glLoadMatrixTransposef(GLfloat *m)
{
    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    wes_transpose4(m, m_current->data);
    wes_classify(m_current);
}

GLvoid
glMultMatrixf(GLfloat *m)
{
    matrix4_t   mat_m[1], mat_r[1];

    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);

    wes_assign(m, mat_m);
    wes_classify(mat_m);
    wes_mul4(m_current, mat_m, mat_r);
    *m_current = *mat_r;
}

GLvoid
glMultMatrixTransposef(GLfloat *m)
{
    GLfloat tmp[16];
    wes_transpose4(m, tmp);
    glMultMatrixf(tmp);
}


GLvoid
glLoadIdentity()
{
    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    wes_identity(m_current);
}


GLvoid
glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    if (x == 0.0f && y == 0.0f && z == 0.0f){
        return;
    }
    if (x == 0.0f){
        if (y == 0.0f){
            wes_rotate_z(angle, z);
            return;
        } else if (z == 0.0f){
            wes_rotate_y(angle, y);
            return;
        }
    } else if (y == 0.0f){
        if (z == 0.0f){
            wes_rotate_x(angle, x);
            return;
        }
    }
    wes_rotate_xyz(angle, x, y, z);
}

GLvoid
glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    if (m_current->flags == WES_M_IDENTITY){
        m_current->data[12] = x;
        m_current->data[13] = y;
        m_current->data[14] = z;
        m_current->flags |= WES_M_TRANSLATED;
    } else if (m_current->flags == (WES_M_IDENTITY | WES_M_TRANSLATED)){
        m_current->data[12] += x;
        m_current->data[13] += y;
        m_current->data[14] += z;
    } else if (m_current->flags == WES_M_SCALED){
        m_current->data[12] = x * m_current->data[0];
        m_current->data[13] = y * m_current->data[5];
        m_current->data[14] = z * m_current->data[10];
        m_current->flags |= WES_M_TRANSLATED;
    } else if (m_current->flags == (WES_M_SCALED | WES_M_TRANSLATED)){
        m_current->data[12] += x * m_current->data[0];
        m_current->data[13] += y * m_current->data[5];
        m_current->data[14] += z * m_current->data[10];
    } else {
        m_current->data[12] += x * m_current->data[0] + y * m_current->data[4] + z * m_current->data[8];
        m_current->data[13] += x * m_current->data[1] + y * m_current->data[5] + z * m_current->data[9];
        m_current->data[14] += x * m_current->data[2] + y * m_current->data[6] + z * m_current->data[10];
        m_current->data[15] += x * m_current->data[3] + y * m_current->data[7] + z * m_current->data[11];
        m_current->flags |= WES_M_TRANSLATED;
    }
}

GLvoid
glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    if (m_current->flags == WES_M_IDENTITY || m_current->flags == (WES_M_IDENTITY | WES_M_TRANSLATED)){
        m_current->data[0] = x; m_current->data[5] = y; m_current->data[10] = z;
        m_current->flags = WES_M_SCALED | (m_current->flags & WES_M_TRANSLATED);
    } else if (m_current->flags == WES_M_SCALED || m_current->flags == (WES_M_SCALED | WES_M_TRANSLATED)){
        m_current->data[0] *= x; m_current->data[5] *= y; m_current->data[10] *= z;
    } else {
        m_current->data[0] *= x; m_current->data[1] *= x; m_current->data[2] *= x; m_current->data[3] *= x;
        m_current->data[4] *= y; m_current->data[5] *= y; m_current->data[6] *= y; m_current->data[7] *= y;
        m_current->data[8] *= z; m_current->data[9] *= z; m_current->data[10] *= z; m_current->data[11] *= z;
        m_current->flags |= WES_M_SCALED;
    }
}

GLvoid
glFrustrumf(float l, float r, float b, float t, float n, float f)
{
    GLfloat m0, m5, m8, m9, m10, m14;
    GLfloat mc8, mc9, mc10, mc11;

    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    m0  = 2 * n / (r - l);
    m5  = 2 * n / (t - b);
    m8  = (r + l) / (r - l);
    m9  = (t + b) / (t - b);
    m10 = - (f + n) / (f - n);
    m14 = - 2 * f * n / (f - n);

    m_current->flags |= WES_M_DIRTY;

    mc8 = m_current->data[8];
    mc9 = m_current->data[9];
    mc10 = m_current->data[10];
    mc11 = m_current->data[11];

    /* Reordered to minimize temporary variables */
    m_current->data[8]  = m8 * m_current->data[0] + m9 * m_current->data[4] + m10 * m_current->data[8] - m_current->data[12];
	m_current->data[9]  = m8 * m_current->data[1] + m9 * m_current->data[5] + m10 * m_current->data[9] - m_current->data[13];
	m_current->data[10] = m8 * m_current->data[2] + m9 * m_current->data[6] + m10 * m_current->data[10] - m_current->data[14];
	m_current->data[11] = m8 * m_current->data[3] + m9 * m_current->data[7] + m10 * m_current->data[11] - m_current->data[15];
    m_current->data[0] *= m0;
	m_current->data[1] *= m0;
	m_current->data[2] *= m0;
	m_current->data[3] *= m0;
	m_current->data[4] *= m5;
	m_current->data[5] *= m5;
	m_current->data[6] *= m5;
	m_current->data[7] *= m5;
	m_current->data[12] = m14 * mc8;
	m_current->data[13] = m14 * mc9;
	m_current->data[14] = m14 * mc10;
	m_current->data[15] = m14 * mc11;
}

GLvoid
glOrthof(float l, float r, float b, float t, float n, float f)
{
    GLfloat m0  = 2 / (r - l);
    GLfloat m5  = 2 / (t - b);
    GLfloat m10 = - 2 / (f - n);
    GLfloat m12 = - (r + l) / (r - l);
    GLfloat m13 = - (t + b) / (t - b);
    GLfloat m14 = - (f + n) / (f - n);

    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    m_current->flags |= WES_M_DIRTY;

    /* Reordered to minimize temporary variables */
	m_current->data[12] += m12 * m_current->data[0] + m13 * m_current->data[4] + m14 * m_current->data[8];
	m_current->data[13] += m12 * m_current->data[1] + m13 * m_current->data[5] + m14 * m_current->data[9];
	m_current->data[14] += m12 * m_current->data[2] + m13 * m_current->data[6] + m14 * m_current->data[10];
	m_current->data[15] += m12 * m_current->data[3] + m13 * m_current->data[7] + m14 * m_current->data[11];
    m_current->data[0]  *= m0;
    m_current->data[1]  *= m0;
	m_current->data[2]  *= m0;
	m_current->data[3]  *= m0;
	m_current->data[4]  *= m5;
	m_current->data[5]  *= m5;
	m_current->data[6]  *= m5;
	m_current->data[7]  *= m5;
	m_current->data[8]  *= m10;
	m_current->data[9]  *= m10;
	m_current->data[10] *= m10;
	m_current->data[11] *= m10;

}

GLvoid
glPushMatrix()
{
    int i;
    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    switch(m_mode)
    {
        case GL_MODELVIEW:
            for(i = 0; i < m_modelview_num; i++){
                m_modelview[i + 1] = m_modelview[i];
            }
            if (m_modelview_num < WES_MODELVIEW_NUM - 1){
                m_modelview_num++;
            }
            break;

        case GL_PROJECTION:
            for(i = 0; i < m_projection_num; i++){
                m_projection[i + 1] = m_projection[i];
            }
            if (m_projection_num < WES_PROJECTION_NUM - 1){
                m_projection_num++;
            }
            break;

        case GL_TEXTURE:
            for(i = 0; i < m_texture_num; i++){
                m_texture[i + 1] = m_texture[i];
            }
            if (m_texture_num < WES_TEXTURE_NUM - 1){
                m_texture_num++;
            }
            break;

        case GL_COLOR:
            for(i = 0; i < m_color_num; i++){
                m_color[i + 1] = m_color[i];
            }
            if (m_color_num < WES_TEXTURE_NUM - 1){
                m_color_num++;
            }
            break;

        default:
            break;
    }
}

GLvoid
glPopMatrix()
{
    int i;
    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    switch(m_mode)
    {
        case GL_MODELVIEW:
            if (m_modelview_num > 1){
                for(i = 1; i < m_modelview_num; i++){
                    m_modelview[i - 1] = m_modelview[i];
                }
                m_modelview_num--;
            }
            break;

        case GL_PROJECTION:
            if (m_projection_num > 1){
                for(i = 1; i < m_projection_num; i++){
                    m_projection[i - 1] = m_projection[i];
                }
                m_projection_num--;
            }
            break;

        case GL_TEXTURE:
            if (m_texture_num > 1){
                for(i = 1; i < m_texture_num; i++){
                    m_texture[i - 1] = m_texture[i];
                }
                m_texture_num--;
            }
            break;

        case GL_COLOR:
            if (m_color_num > 1){
                for(i = 1; i < m_color_num; i++){
                    m_color[i - 1] = m_color[i];
                }
                m_color_num--;
            }
            break;

        default:
            break;
    }
}

GLvoid
glActiveTexture(GLenum texture)
{
    m_activetexture = texture;
    wes_gl->glActiveTexture(texture);
}

GLvoid
gluOrtho2D(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top)
{
    glOrthof(left, right, bottom, top, -1, 1);
}

GLvoid
gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat znear, GLfloat zfar)
{

    GLfloat rad = fovy * M_PI / 360;
    GLfloat sine, cot, dz, idz;
    GLfloat m0, m10, m14;
    GLfloat tmp[4];

    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);

    dz = znear - zfar;
    sine = sin(rad);
    if ((dz == 0) || (sine == 0) || (aspect == 0)) {
        return;
    }
    cot = cos(rad) / sine;
    idz = 1.0f / dz;

    m0 = cot / aspect;
    m10 = (zfar + znear) * idz;
    m14 = 2 * znear * zfar * idz;

    if (m_current->flags == WES_M_IDENTITY){
        m_current->data[0] = m0;
        m_current->data[5] = cot;
        m_current->data[10] = m10;
        m_current->data[11] = -1.0f;
        m_current->data[14] = m14;
        m_current->data[15] = 0.0;
    } else {
        tmp[0] = m_current->data[8];
        tmp[1] = m_current->data[9];
        tmp[2] = m_current->data[10];
        tmp[3] = m_current->data[11];
        m_current->data[8]  = m10 * m_current->data[8] - m_current->data[12];
        m_current->data[9]  = m10 * m_current->data[9] - m_current->data[13];
        m_current->data[10] = m10 * m_current->data[10] - m_current->data[14];
        m_current->data[11] = m10 * m_current->data[11] - m_current->data[15];
        m_current->data[0] *= m0;
        m_current->data[1] *= m0;
        m_current->data[2] *= m0;
        m_current->data[3] *= m0;
        m_current->data[4] *= cot;
        m_current->data[5] *= cot;
        m_current->data[6] *= cot;
        m_current->data[7] *= cot;
        m_current->data[12] = m14 * tmp[0];
        m_current->data[13] = m14 * tmp[1];
        m_current->data[14] = m14 * tmp[2];
        m_current->data[15] = m14 * tmp[3];
    }
    m_current->flags |= WES_M_DIRTY;
}

GLvoid
gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
          GLfloat centerx, GLfloat centery, GLfloat centerz,
          GLfloat upx, GLfloat upy, GLfloat upz)
{
    GLfloat forward[3], side[3], up[3];
    GLfloat tmp[12];

    m_modelview_mod |= (m_mode == GL_MODELVIEW);
    m_projection_mod |= (m_mode == GL_PROJECTION);
    m_current->flags |= WES_M_DIRTY;

    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    normalize3(forward);
    cross3(forward, up, side);
    normalize3(side);
    cross3(side, forward, up);

    tmp[0]  = side[0] * m_current->data[0] + side[1] * m_current->data[4] + side[2] * m_current->data[8];
	tmp[1]  = side[0] * m_current->data[1] + side[1] * m_current->data[5] + side[2] * m_current->data[9];
	tmp[2]  = side[0] * m_current->data[2] + side[1] * m_current->data[6] + side[2] * m_current->data[10];
	tmp[3]  = side[0] * m_current->data[3] + side[1] * m_current->data[7] + side[2] * m_current->data[11];
	tmp[4]  = up[0] * m_current->data[0] + up[1] * m_current->data[4] + up[2] * m_current->data[8];
	tmp[5]  = up[0] * m_current->data[1] + up[1] * m_current->data[5] + up[2] * m_current->data[9];
	tmp[6]  = up[0] * m_current->data[2] + up[1] * m_current->data[6] + up[2] * m_current->data[10];
	tmp[7]  = up[0] * m_current->data[3] + up[1] * m_current->data[7] + up[2] * m_current->data[11];
	tmp[8]  = - forward[0] * m_current->data[0] - forward[1] * m_current->data[4] - forward[2] * m_current->data[8];
	tmp[9]  = - forward[0] * m_current->data[1] - forward[1] * m_current->data[5] - forward[2] * m_current->data[9];
	tmp[10] = - forward[0] * m_current->data[2] - forward[1] * m_current->data[6] - forward[2] * m_current->data[10];
	tmp[11] = - forward[0] * m_current->data[3] - forward[1] * m_current->data[7] - forward[2] * m_current->data[11];
    memcpy(m_current->data, tmp, 12 * sizeof(GLfloat));
    glTranslatef(-eyex, -eyey, -eyez);
}

