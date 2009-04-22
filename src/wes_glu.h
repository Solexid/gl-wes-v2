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

extern GLvoid gluOrtho2D(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top);
extern GLvoid gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat znear, GLfloat zfar);
extern GLvoid gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx, GLfloat centery,
                        GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz);

extern GLvoid gluBuild2DMipmaps(GLenum target, GLint components, GLsizei width, GLsizei height,
                                GLenum format, GLenum type, const GLvoid *pixels );
