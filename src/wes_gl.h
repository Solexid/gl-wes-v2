

#ifndef __WES_GL_H__
#define __WES_GL_H__

#include "wes.h"
#include "wes_gl_defines.h"

/* pass throughs */
//#define glActiveTexture             wes_gl->glActiveTexture
#define glAttachShader              wes_gl->glAttachShader
#define glBindAttribLocation		wes_gl->glBindAttribLocation
#define glBindBuffer			    wes_gl->glBindBuffer
#define glBindFramebuffer			wes_gl->glBindFramebuffer
#define glBindRenderbuffer			wes_gl->glBindRenderbuffer
#define glBindTexture			    wes_gl->glBindTexture
#define glBlendColor			    wes_gl->glBlendColor
#define glBlendEquation			    wes_gl->glBlendEquation
#define glBlendEquationSeparate		wes_gl->glBlendEquationSeparate
#define glBlendFunc			        wes_gl->glBlendFunc
#define glBlendFuncSeparate			wes_gl->glBlendFuncSeparate
#define glBufferData			    wes_gl->glBufferData
#define glBufferSubData			    wes_gl->glBufferSubData
#define glCheckFramebufferStatus	wes_gl->glCheckFramebufferStatus
#define glClear			            wes_gl->glClear
#define glClearColor			    wes_gl->glClearColor
#define glClearDepth			    glClearDepthf
#define glClearDepthf			    wes_gl->glClearDepthf
#define glClearStencil			    wes_gl->glClearStencil
#define glColorMask			        wes_gl->glColorMask
#define glCompileShader			    wes_gl->glCompileShader
#define glCompressedTexImage2D		wes_gl->glCompressedTexImage2D
#define glCompressedTexSubImage2D	wes_gl->glCompressedTexSubImage2D
#define glCopyTexImage2D			wes_gl->glCopyTexImage2D
#define glCopyTexSubImage2D			wes_gl->glCopyTexSubImage2D
#define glCreateProgram			    wes_gl->glCreateProgram
#define glCreateShader			    wes_gl->glCreateShader
#define glCullFace			        wes_gl->glCullFace
#define glDeleteBuffers		    	wes_gl->glDeleteBuffers
#define glDeleteFramebuffers		wes_gl->glDeleteFramebuffers
#define glDeleteTextures			wes_gl->glDeleteTextures
#define glDeleteProgram			    wes_gl->glDeleteProgram
#define glDeleteRenderbuffers		wes_gl->glDeleteRenderbuffers
#define glDeleteShader			    wes_gl->glDeleteShader
#define glDetachShader			    wes_gl->glDetachShader
#define glDepthFunc			        wes_gl->glDepthFunc
#define glDepthMask			        wes_gl->glDepthMask
#define glDepthRangef		       	wes_gl->glDepthRangef
//#define glDisable			        wes_gl->glDisable
#define glDisableVertexAttribArray	wes_gl->glDisableVertexAttribArray
#define glDrawArrays			    wes_gl->glDrawArrays
#define glDrawElements			    wes_gl->glDrawElements
//#define glEnable			        wes_gl->glEnable
#define glEnableVertexAttribArray	wes_gl->glEnableVertexAttribArray
#define glFinish			        wes_gl->glFinish
#define glFlush			            wes_gl->glFlush
#define glFramebufferRenderbuffer	wes_gl->glFramebufferRenderbuffer
#define glFramebufferTexture2D		wes_gl->glFramebufferTexture2D
#define glFrontFace			        wes_gl->glFrontFace
#define glGenBuffers			    wes_gl->glGenBuffers
#define glGenerateMipmap			wes_gl->glGenerateMipmap
#define glGenFramebuffers			wes_gl->glGenFramebuffers
#define glGenRenderbuffers			wes_gl->glGenRenderbuffers
#define glGenTextures			    wes_gl->glGenTextures
#define glGetActiveAttrib			wes_gl->glGetActiveAttrib
#define glGetActiveUniform			wes_gl->glGetActiveUniform
#define glGetAttachedShaders		wes_gl->glGetAttachedShaders
#define glGetAttribLocation			wes_gl->glGetAttribLocation
#define glGetBooleanv			    wes_gl->glGetBooleanv
#define glGetBufferParameteriv		wes_gl->glGetBufferParameteriv
#define glGetError			        wes_gl->glGetError
#define glGetFloatv			        wes_gl->glGetFloatv
#define glGetFramebufferAttachmentParameteriv   wes_gl->glGetFramebufferAttachmentParameteriv
#define glGetIntegerv			    wes_gl->glGetIntegerv
#define glGetProgramiv			    wes_gl->glGetProgramiv
#define glGetProgramInfoLog			wes_gl->glGetProgramInfoLog
#define glGetRenderbufferParameteriv    wes_gl->glGetRenderbufferParameteriv
#define glGetShaderiv			    wes_gl->glGetShaderiv
#define glGetShaderInfoLog			wes_gl->glGetShaderInfoLog
#define glGetShaderPrecisionFormat	wes_gl->glGetShaderPrecisionFormat
#define glGetShaderSource			wes_gl->glGetShaderSource
#define glGetString			        wes_gl->glGetString
#define glGetTexParameterfv			wes_gl->glGetTexParameterfv
#define glGetTexParameteriv			wes_gl->glGetTexParameteriv
#define glGetUniformfv			    wes_gl->glGetUniformfv
#define glGetUniformiv			    wes_gl->glGetUniformiv
#define glGetUniformLocation		wes_gl->glGetUniformLocation
#define glGetVertexAttribfv			wes_gl->glGetVertexAttribfv
#define glGetVertexAttribiv			wes_gl->glGetVertexAttribiv
#define glGetVertexAttribPointerv	wes_gl->glGetVertexAttribPointerv
#define glHint			            wes_gl->glHint
#define glIsBuffer			        wes_gl->glIsBuffer
#define glIsEnabled			        wes_gl->glIsEnabled
#define glIsFramebuffer			    wes_gl->glIsFramebuffer
#define glIsProgram			        wes_gl->glIsProgram
#define glIsRenderbuffer			wes_gl->glIsRenderbuffer
#define glIsShader			        wes_gl->glIsShader
#define glIsTexture			        wes_gl->glIsTexture
#define glLineWidth			        wes_gl->glLineWidth
#define glLinkProgram			    wes_gl->glLinkProgram
#define glPixelStorei			    wes_gl->glPixelStorei
#define glPolygonOffset			    wes_gl->glPolygonOffset
#define glReadPixels			    wes_gl->glReadPixels
#define glReleaseShaderCompiler		wes_gl->glReleaseShaderCompiler
#define glRenderbufferStorage		wes_gl->glRenderbufferStorage
#define glSampleCoverage			wes_gl->glSampleCoverage
#define glScissor			        wes_gl->glScissor
#define glShaderBinary			    wes_gl->glShaderBinary
#define glShaderSource			    wes_gl->glShaderSource
#define glStencilFunc			    wes_gl->glStencilFunc
#define glStencilFuncSeparate		wes_gl->glStencilFuncSeparate
#define glStencilMask			    wes_gl->glStencilMask
#define glStencilMaskSeparate		wes_gl->glStencilMaskSeparate
#define glStencilOp			        wes_gl->glStencilOp
#define glStencilOpSeparate			wes_gl->glStencilOpSeparate
//#define glTexImage2D			    wes_gl->glTexImage2D
#define glTexParameterf			    wes_gl->glTexParameterf
#define glTexParameterfv			wes_gl->glTexParameterfv
#define glTexParameteri			    wes_gl->glTexParameteri
#define glTexParameteriv			wes_gl->glTexParameteriv
#define glTexSubImage2D			    wes_gl->glTexSubImage2D
#define glUniform1f			        wes_gl->glUniform1f
#define glUniform1fv			    wes_gl->glUniform1fv
#define glUniform1i			        wes_gl->glUniform1i
#define glUniform1iv			    wes_gl->glUniform1iv
#define glUniform2f			        wes_gl->glUniform2f
#define glUniform2fv			    wes_gl->glUniform2fv
#define glUniform2i			        wes_gl->glUniform2i
#define glUniform2iv			    wes_gl->glUniform2iv
#define glUniform3f			        wes_gl->glUniform3f
#define glUniform3fv			    wes_gl->glUniform3fv
#define glUniform3i			        wes_gl->glUniform3i
#define glUniform3iv			    wes_gl->glUniform3iv
#define glUniform4f			        wes_gl->glUniform4f
#define glUniform4fv			    wes_gl->glUniform4fv
#define glUniform4i			        wes_gl->glUniform4i
#define glUniform4iv			    wes_gl->glUniform4iv
//#define glUniformMatrix2fv			wes_gl->glUniformMatrix2fv
//#define glUniformMatrix3fv			wes_gl->glUniformMatrix3fv
//#define glUniformMatrix4fv			wes_gl->glUniformMatrix4fv
#define glUseProgram			    wes_gl->glUseProgram
#define glValidateProgram			wes_gl->glValidateProgram
#define glVertexAttrib1f			wes_gl->glVertexAttrib1f
#define glVertexAttrib1fv			wes_gl->glVertexAttrib1fv
#define glVertexAttrib2f			wes_gl->glVertexAttrib2f
#define glVertexAttrib2fv			wes_gl->glVertexAttrib2fv
#define glVertexAttrib3f			wes_gl->glVertexAttrib3f
#define glVertexAttrib3fv			wes_gl->glVertexAttrib3fv
#define glVertexAttrib4f			wes_gl->glVertexAttrib4f
#define glVertexAttrib4fv			wes_gl->glVertexAttrib4fv
#define glVertexAttribPointer		wes_gl->glVertexAttribPointer
#define glViewport                  wes_gl->glViewport


extern GLvoid glEnable(GLenum e);
extern GLvoid glDisable(GLenum e);

/*  Alpha Test                      */
extern GLvoid   glAlphaFunc(GLenum func, GLclampf ref);

/*  Begin / End Paradigm            */
extern GLvoid   glBegin(GLenum mode);
extern GLvoid   glEnd();

/*  Vertex Specification            */
extern GLvoid   glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
extern GLvoid   glVertex3f(GLfloat x, GLfloat y, GLfloat z);
extern GLvoid   glVertex2f(GLfloat x, GLfloat y);
extern GLvoid   glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
extern GLvoid   glTexCoord3f(GLfloat s, GLfloat t, GLfloat r);
extern GLvoid   glTexCoord2f(GLfloat s, GLfloat t);
extern GLvoid   glTexCoord1f(GLfloat s);
extern GLvoid   glMultiTexCoord4f(GLenum tex, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
extern GLvoid   glMultiTexCoord3f(GLenum tex, GLfloat s, GLfloat t, GLfloat r);
extern GLvoid   glMultiTexCoord2f(GLenum tex, GLfloat s, GLfloat t);
extern GLvoid   glMultiTexCoord1f(GLenum tex, GLfloat s);
extern GLvoid   glNormal3f(GLfloat x, GLfloat y, GLfloat z);
extern GLvoid   glFogCoordf(GLfloat f);
extern GLvoid   glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
extern GLvoid   glColor3f(GLfloat r, GLfloat g, GLfloat b);
extern GLvoid   glSecondaryColor3f(GLfloat r, GLfloat g, GLfloat b);

/* Vertex Array Specification */
extern GLvoid   glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
extern GLvoid   glNormalPointer(GLenum type, GLsizei stride, const GLvoid *ptr);
extern GLvoid   glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
extern GLvoid   glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
extern GLvoid   glSecondaryColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
extern GLvoid   glFogCoordPointer(GLenum type, GLsizei stride, const GLvoid *ptr);

/*  Coordinate Transformations  */
extern GLvoid   glMatrixMode(GLenum mode);
extern GLvoid   glLoadMatrixf(GLfloat *m);
extern GLvoid   glLoadMatrixTransposef(GLfloat *m);
extern GLvoid   glMultMatrixf(GLfloat *m);
extern GLvoid   glMultMatrixTransposef(GLfloat *m);
extern GLvoid   glLoadIdentity();
extern GLvoid   glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
extern GLvoid   glTranslatef(GLfloat x, GLfloat y, GLfloat z);
extern GLvoid   glScalef(GLfloat x, GLfloat y, GLfloat z);
extern GLvoid   glFrustrumf(float l, float r, float b, float t, float n, float f);
extern GLvoid   glOrthof(float l, float r, float b, float t, float n, float f);
extern GLvoid   glPushMatrix();
extern GLvoid   glPopMatrix();
extern GLvoid   glActiveTexture(GLenum texture);

/*  Vertex Shaders              */
extern GLvoid   glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
extern GLvoid   glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
extern GLvoid   glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);

/* Light  */
extern GLvoid   glLightf(GLenum light, GLenum pname, GLfloat params);
extern GLvoid   glLightfv(GLenum light, GLenum pname, GLfloat *params);
extern GLvoid   glLightModeli(GLenum pname, GLint params);
extern GLvoid   glLightModelfv(GLenum pname, GLfloat *params);
extern GLvoid   glMaterialf(GLenum face, GLenum pname, GLfloat params);
extern GLvoid   glMaterialfv(GLenum face, GLenum pname, GLfloat *params);

/* Fog  */
extern GLvoid   glFogi(GLenum pname, GLint param);
extern GLvoid   glFogf(GLenum pname, GLfloat param);
extern GLvoid   glFogfv(GLenum pname, GLfloat *param);

/* Texture  */
extern GLvoid   glTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height,
           GLint border, GLenum format, GLenum type, const GLvoid *pixels);

#endif
