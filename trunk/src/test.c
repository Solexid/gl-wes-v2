

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <EGL/egl.h>
#include "wes_gl.h"
#include "wes_glu.h"

GLfloat     x = 0, y = 0, z = -5.0;
uint32_t    loop = 1;

EGLint		VersionMajor;
EGLint		VersionMinor;

EGLDisplay  Display;
EGLContext  Context;
EGLConfig   Config;
EGLSurface  Surface;

EGLNativeDisplayType    Device;
EGLNativeWindowType     Handle;

const EGLint ConfigAttribs[] = {
	EGL_LEVEL,				0,
	EGL_DEPTH_SIZE,         16,
	EGL_STENCIL_SIZE,       8,
	EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
	EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
	EGL_NATIVE_RENDERABLE,	EGL_FALSE,
	EGL_NONE
};

const EGLint ContextAttribs[] = {
	EGL_CONTEXT_CLIENT_VERSION, 	2,
	EGL_NONE
};

const char* EGLErrorString(){
	EGLint nErr = eglGetError();
	switch(nErr){
		case EGL_SUCCESS: 				return "EGL_SUCCESS";
		case EGL_BAD_DISPLAY:			return "EGL_BAD_DISPLAY";
		case EGL_NOT_INITIALIZED:		return "EGL_NOT_INITIALIZED";
		case EGL_BAD_ACCESS:			return "EGL_BAD_ACCESS";
		case EGL_BAD_ALLOC:				return "EGL_BAD_ALLOC";
		case EGL_BAD_ATTRIBUTE:			return "EGL_BAD_ATTRIBUTE";
		case EGL_BAD_CONFIG:			return "EGL_BAD_CONFIG";
		case EGL_BAD_CONTEXT:			return "EGL_BAD_CONTEXT";
		case EGL_BAD_CURRENT_SURFACE:	return "EGL_BAD_CURRENT_SURFACE";
		case EGL_BAD_MATCH:				return "EGL_BAD_MATCH";
		case EGL_BAD_NATIVE_PIXMAP:		return "EGL_BAD_NATIVE_PIXMAP";
		case EGL_BAD_NATIVE_WINDOW:		return "EGL_BAD_NATIVE_WINDOW";
		case EGL_BAD_PARAMETER:			return "EGL_BAD_PARAMETER";
		case EGL_BAD_SURFACE:			return "EGL_BAD_SURFACE";
		default:						return "unknown";
	}
}

void
context_open(uint32_t w, uint32_t h, uint32_t vsync)
{
    Device = (EGLNativeDisplayType) EGL_DEFAULT_DISPLAY;
    EGLint nConfigs;

    SDL_Init(SDL_INIT_EVERYTHING);
	if (SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE) == NULL){
		PRINT_ERROR("\nCould not set Video Mode");
	}

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWMInfo(&info);
	Handle = (EGLNativeWindowType) info.window;
	Device = GetDC(Handle);
    Display = eglGetDisplay((EGLNativeDisplayType) Device);

    if (Display == EGL_NO_DISPLAY){
        PRINT_ERROR("\nDisplay Get failed: %s", EGLErrorString());
    }

    if (!eglInitialize(Display, &VersionMajor, &VersionMinor)){
        PRINT_ERROR("\nDisplay Initialize failed: %s", EGLErrorString());
    }

    if (!eglChooseConfig(Display, ConfigAttribs, &Config, 1, &nConfigs)){
        PRINT_ERROR("\nConfiguration failed: %s", EGLErrorString());
    } else if (nConfigs != 1){
        PRINT_ERROR("\nConfiguration failed: nconfig %i, %s", nConfigs, EGLErrorString());
    }

    Surface = eglCreateWindowSurface(Display, Config, Handle, NULL);
    if (Surface == EGL_NO_SURFACE){
		PRINT_ERROR("\nSurface Creation failed: %s will attempt without window...", EGLErrorString());
        Surface = eglCreateWindowSurface(Display, Config, NULL, NULL);
        if (Surface == EGL_NO_SURFACE){
            PRINT_ERROR("\nSurface Creation failed: %s", EGLErrorString());
        }
    }
    eglBindAPI(EGL_OPENGL_ES_API);

    Context = eglCreateContext(Display, Config, EGL_NO_CONTEXT, ContextAttribs);
    if (Context == EGL_NO_CONTEXT){
        PRINT_ERROR("\nContext Creation failed: %s", EGLErrorString());
    }

    if (!eglMakeCurrent(Display, Surface, Surface, Context)){
        PRINT_ERROR("\nMake Current failed: %s", EGLErrorString());
    };
    eglSwapInterval(Display, vsync);
}

void
context_handlekey(SDL_KeyboardEvent *key)
{
    switch(key->keysym.sym){
        case SDLK_ESCAPE:
            loop = 0;
            break;
        case SDLK_UP:
            y += 0.5;
            break;
        case SDLK_DOWN:
            y -= 0.5;
            break;
        case SDLK_LEFT:
            x -= 0.5;
            break;
        case SDLK_RIGHT:
            x += 0.5;
            break;
        case SDLK_PAGEUP:
            z -= 0.5;
            break;
        case SDLK_PAGEDOWN:
            z += 0.5;
            break;
        default:
            break;
    }

}

void
context_update()
{
    eglSwapBuffers(Display, Surface);

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type){
            case SDL_QUIT: {
                loop = 0;
                break;
            }
            case SDL_KEYDOWN:
                context_handlekey(&event.key);
                break;
        }
    }
}

void
context_close()
{
	eglSwapBuffers(Display, Surface);
	eglMakeCurrent(Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
 	eglDestroyContext(Display, Context);
	eglDestroySurface(Display, Surface);
   	eglTerminate(Display);
}

GLuint texture; /* Storage For One Texture ( NEW ) */

int LoadGLTextures( )
{
    int Status = FALSE;
    SDL_Surface *image;
    if ((image = SDL_LoadBMP( "./data/crate.bmp"))){
	    Status = TRUE;
	    glGenTextures( 1, &texture );
	    glBindTexture( GL_TEXTURE_2D, texture );
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_BGR, image->w, image->h, 0, GL_BGR, GL_UNSIGNED_BYTE, image->pixels );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        }

    if (image){
	    SDL_FreeSurface(image);
    }
    return Status;
}

GLfloat xrot, yrot, zrot;

void
draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* Move Into The Screen 5 Units */
    glLoadIdentity();
    glTranslatef( x, y, z );

    glRotatef( xrot, 1.0f, 0.0f, 0.0f); /* Rotate On The X Axis */
    glRotatef( yrot, 0.0f, 1.0f, 0.0f); /* Rotate On The Y Axis */
    glRotatef( zrot, 0.0f, 0.0f, 1.0f); /* Rotate On The Z Axis */

    /* Select Our Texture */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glColor3f(1,1,1);
    glBegin(GL_QUADS);

      glNormal3f( 0.0f, 0.0f, 1.0f );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -1.0f, -1.0f,  1.0f );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f(  1.0f, -1.0f,  1.0f );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f(  1.0f,  1.0f,  1.0f );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -1.0f,  1.0f,  1.0f );

      glNormal3f( 0.0f, 0.0f, -1.0f);
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f, -1.0f, -1.0f );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f,  1.0f, -1.0f );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  1.0f,  1.0f, -1.0f );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  1.0f, -1.0f, -1.0f );

      glNormal3f( 0.0f, 1.0f, 0.0f );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -1.0f,  1.0f, -1.0f );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -1.0f,  1.0f,  1.0f );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f(  1.0f,  1.0f,  1.0f );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f(  1.0f,  1.0f, -1.0f );

      glNormal3f( 0.0f, -1.0f, 0.0f );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f, -1.0f, -1.0f );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  1.0f, -1.0f, -1.0f );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  1.0f, -1.0f,  1.0f );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f, -1.0f,  1.0f );

      glNormal3f( 1.0f, 0.0f, 0.0f);
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( 1.0f, -1.0f, -1.0f );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( 1.0f,  1.0f, -1.0f );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( 1.0f,  1.0f,  1.0f );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( 1.0f, -1.0f,  1.0f );

      glNormal3f( -1.0f, 0.0f, 0.0f );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -1.0f, -1.0f, -1.0f );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f, -1.0f,  1.0f );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f,  1.0f,  1.0f );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -1.0f,  1.0f, -1.0f );
    glEnd( );
    xrot += 0.3f; /* X Axis Rotation */
    yrot += 0.2f; /* Y Axis Rotation */
    zrot += 0.4f; /* Z Axis Rotation */

    /*
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(-1.5f,0.0f,-6.0f);						// Move Left 1.5 Units And Into The Screen 6.0
	glRotatef(rtri,0.0f,1.0f,0.0f);						// Rotate The Triangle On The Y axis ( NEW )
	glBegin(GL_TRIANGLES);								// Start Drawing A Triangle
		glColor3f(1.0f,0.0f,0.0f);						// Red
		glVertex3f( 0.0f, 1.0f, 0.0f);					// Top Of Triangle (Front)
		glColor3f(0.0f,1.0f,0.0f);						// Green
		glVertex3f(-1.0f,-1.0f, 1.0f);					// Left Of Triangle (Front)
		glColor3f(0.0f,0.0f,1.0f);						// Blue
		glVertex3f( 1.0f,-1.0f, 1.0f);					// Right Of Triangle (Front)
		glColor3f(1.0f,0.0f,0.0f);						// Red
		glVertex3f( 0.0f, 1.0f, 0.0f);					// Top Of Triangle (Right)
		glColor3f(0.0f,0.0f,1.0f);						// Blue
		glVertex3f( 1.0f,-1.0f, 1.0f);					// Left Of Triangle (Right)
		glColor3f(0.0f,1.0f,0.0f);						// Green
		glVertex3f( 1.0f,-1.0f, -1.0f);					// Right Of Triangle (Right)
		glColor3f(1.0f,0.0f,0.0f);						// Red
		glVertex3f( 0.0f, 1.0f, 0.0f);					// Top Of Triangle (Back)
		glColor3f(0.0f,1.0f,0.0f);						// Green
		glVertex3f( 1.0f,-1.0f, -1.0f);					// Left Of Triangle (Back)
		glColor3f(0.0f,0.0f,1.0f);						// Blue
		glVertex3f(-1.0f,-1.0f, -1.0f);					// Right Of Triangle (Back)
		glColor3f(1.0f,0.0f,0.0f);						// Red
		glVertex3f( 0.0f, 1.0f, 0.0f);					// Top Of Triangle (Left)
		glColor3f(0.0f,0.0f,1.0f);						// Blue
		glVertex3f(-1.0f,-1.0f,-1.0f);					// Left Of Triangle (Left)
		glColor3f(0.0f,1.0f,0.0f);						// Green
		glVertex3f(-1.0f,-1.0f, 1.0f);					// Right Of Triangle (Left)
	glEnd();											// Done Drawing The Pyramid

    glColor3f(0.0f,1.0f,0.0f);
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(1.5f,0.0f,-7.0f);						// Move Right 1.5 Units And Into The Screen 7.0
	glRotatef(rquad,1.0f,1.0f,1.0f);					// Rotate The Quad On The X axis ( NEW )
	glBegin(GL_QUADS);									// Draw A Quad
		glColor3f(0.0f,1.0f,0.0f);						// Set The Color To Green
		glVertex3f( 1.0f, 1.0f,-1.0f);					// Top Right Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f,-1.0f);					// Top Left Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f, 1.0f);					// Bottom Left Of The Quad (Top)
		glVertex3f( 1.0f, 1.0f, 1.0f);					// Bottom Right Of The Quad (Top)
		glColor3f(1.0f,0.5f,0.0f);						// Set The Color To Orange
		glVertex3f( 1.0f,-1.0f, 1.0f);					// Top Right Of The Quad (Bottom)
		glVertex3f(-1.0f,-1.0f, 1.0f);					// Top Left Of The Quad (Bottom)
		glVertex3f(-1.0f,-1.0f,-1.0f);					// Bottom Left Of The Quad (Bottom)
		glVertex3f( 1.0f,-1.0f,-1.0f);					// Bottom Right Of The Quad (Bottom)
		glColor3f(1.0f,0.0f,0.0f);						// Set The Color To Red
		glVertex3f( 1.0f, 1.0f, 1.0f);					// Top Right Of The Quad (Front)
		glVertex3f(-1.0f, 1.0f, 1.0f);					// Top Left Of The Quad (Front)
		glVertex3f(-1.0f,-1.0f, 1.0f);					// Bottom Left Of The Quad (Front)
		glVertex3f( 1.0f,-1.0f, 1.0f);					// Bottom Right Of The Quad (Front)
		glColor3f(1.0f,1.0f,0.0f);						// Set The Color To Yellow
		glVertex3f( 1.0f,-1.0f,-1.0f);					// Top Right Of The Quad (Back)
		glVertex3f(-1.0f,-1.0f,-1.0f);					// Top Left Of The Quad (Back)
		glVertex3f(-1.0f, 1.0f,-1.0f);					// Bottom Left Of The Quad (Back)
		glVertex3f( 1.0f, 1.0f,-1.0f);					// Bottom Right Of The Quad (Back)
		glColor3f(0.0f,0.0f,1.0f);						// Set The Color To Blue
		glVertex3f(-1.0f, 1.0f, 1.0f);					// Top Right Of The Quad (Left)
		glVertex3f(-1.0f, 1.0f,-1.0f);					// Top Left Of The Quad (Left)
		glVertex3f(-1.0f,-1.0f,-1.0f);					// Bottom Left Of The Quad (Left)
		glVertex3f(-1.0f,-1.0f, 1.0f);					// Bottom Right Of The Quad (Left)
		glColor3f(1.0f,0.0f,1.0f);						// Set The Color To Violet
		glVertex3f( 1.0f, 1.0f,-1.0f);					// Top Right Of The Quad (Right)
		glVertex3f( 1.0f, 1.0f, 1.0f);					// Top Left Of The Quad (Right)
		glVertex3f( 1.0f,-1.0f, 1.0f);					// Bottom Left Of The Quad (Right)
		glVertex3f( 1.0f,-1.0f,-1.0f);					// Bottom Right Of The Quad (Right)
	glEnd();											// Done Drawing The Quad

	rtri+=0.6f;											// Increase The Rotation Variable For The Triangle ( NEW )
	rquad-=0.45f;										// Decrease The Rotation Variable For The Quad ( NEW )
	*/
}


int
main(int argc, char* argv[])
{
    freopen("stdout.txt", "w", stdout);
    freopen("stderr.txt", "w", stderr);
    fprintf(stdout, "Stdout\n"); fflush(stdout);
    fprintf(stderr, "stderr\n"); fflush(stderr);

    context_open(640, 480, 0);
    wes_init();


    glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDepthRangef(0, 1.0);
    glViewport(0, 0, 640, 480);

    GLfloat col[] = {1,1,1,1};
    glFogi(GL_FOG_COORD_SRC, 0);		// Fog Mode
    glFogi(GL_FOG_MODE, GL_LINEAR);		// Fog Mode
    glFogfv(GL_FOG_COLOR, col);			// Set Fog Color
    glFogf(GL_FOG_DENSITY, 0.35f);				// How Dense Will The Fog Be
    glFogf(GL_FOG_START, 5.0f);				// Fog Start Depth
    glFogf(GL_FOG_END, 10.0f);				// Fog End Depth
    glEnable(GL_FOG);					// Enables GL_FOG


    GLfloat amb[] = {0,0,0,0};
    GLfloat diff[] = {1,1,1,1};
    GLfloat pos[] = {0,0,0,0};
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);				// Setup The Ambient Light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);				// Setup The Ambient Light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, pos);				// Setup The Ambient Light
    //glEnable(GL_LIGHTING);


    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 640.0 / 480.0, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);

    if (!LoadGLTextures()){
        PRINT_ERROR("hmmm");
    }

    while(loop){
        draw();
        context_update();
    }
    wes_destroy();
    context_close();
    return 0;
}

