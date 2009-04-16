

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <EGL/egl.h>
#include "wes_gl.h"
#include "wes_glu.h"

#include "wes_matrix.h"

GLfloat     xrot = 45, yrot, zrot;
GLfloat     x = 0, y = 0, z = -5.0;
GLfloat     xspeed = 0, yspeed = 0, zspeed = 0;
uint32_t    frames = 0;
GLfloat     delta = 0;
uint32_t    t0 = 0;

uint32_t    loop = 1;

uint8_t     *Keyboard;

GLfloat     lightpos[4] = {0, 0, 0, 1};


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

    int num;
    SDL_GetKeyState(&num);
    Keyboard = malloc(num * sizeof(uint8_t));

}

void
context_handlekey()
{

    int num, i;
    uint8_t* key = SDL_GetKeyState(&num);
    for(i = 0; i < num; i++){
        Keyboard[i] = key[i];
    }
    if (Keyboard[SDLK_ESCAPE]){
        loop = 0;
    }
    if (Keyboard[SDLK_UP]){
        y += 0.1 * delta;
    }
    if (Keyboard[SDLK_DOWN]){
        y -= 0.1 * delta;
    }
    if (Keyboard[SDLK_LEFT]){
        x -= 0.1 * delta;
    }
    if (Keyboard[SDLK_RIGHT]){
        x += 0.1 * delta;
    }
    if (Keyboard[SDLK_PAGEUP]){
        z += 0.1 * delta;
    }
    if (Keyboard[SDLK_PAGEDOWN]){
        z -= 0.1 * delta;
    }
    if (Keyboard[SDLK_w]){
        lightpos[1] += 0.1 * delta;
    }
    if (Keyboard[SDLK_s]){
        lightpos[1] -= 0.1 * delta;
    }
    if (Keyboard[SDLK_a]){
        lightpos[0] -= 0.1 * delta;
    }
    if (Keyboard[SDLK_d]){
        lightpos[0] += 0.1 * delta;
    }
}

void
context_update()
{
    eglSwapBuffers(Display, Surface);


    context_handlekey();

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type){
            case SDL_QUIT: {
                loop = 0;
                break;
            }
        }
    }
}

void
context_close()
{
    free(Keyboard);
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



void
draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* Move Into The Screen 5 Units */
    glLoadIdentity();
    //glLightfv(GL_LIGHT1, GL_POSITION, lightpos);				// Setup The Ambient Light

    glTranslatef( x, y, z );

    glRotatef( xrot, 1.0f, 0.0f, 0.0f); /* Rotate On The X Axis */
    glRotatef( yrot, 0.0f, 1.0f, 0.0f); /* Rotate On The Y Axis */
    glRotatef( zrot, 0.0f, 0.0f, 1.0f); /* Rotate On The Z Axis */
    glScalef(1.5,1.5,1.0);

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
    xrot += 0.3 * delta; /* X Axis Rotation */
    yrot += 0.2 * delta; /* Y Axis Rotation */
    zrot += 0.4 * delta; /* Z Axis Rotation */

    frames++;
	GLint t = SDL_GetTicks();
	if (t - t0 > 100){
        GLfloat seconds = (t - t0) / 1000.0;
        GLfloat fps = frames / seconds;
        printf("%g FPS\n", fps);
        delta = 60.0 / fps;
        t0 = t;
        frames  = 0;
	}

    x += xspeed * delta;
    y += yspeed * delta;
    z += zspeed * delta;
}


int
main(int argc, char* argv[])
{
    context_open(640, 480, 0);
        /* Output to console    */
    freopen( "CON", "w", stdout );
    freopen( "CON", "w", stderr );

    wes_init();

    glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDepthRangef(0, 1.0);
    glViewport(0, 0, 640, 480);


    GLfloat col[] = {1.0,1.0,1.0,1.0};
    glFogi(GL_FOG_COORD_SRC, 0);		// Fog Mode
    glFogi(GL_FOG_MODE, GL_LINEAR);		// Fog Mode
    glFogfv(GL_FOG_COLOR, col);			// Set Fog Color
    glFogf(GL_FOG_DENSITY, 0.515f);				// How Dense Will The Fog Be
    glFogf(GL_FOG_START, 5.0f);				// Fog Start Depth
    glFogf(GL_FOG_END, 10.0f);				// Fog End Depth
    //glEnable(GL_FOG);					// Enables GL_FOG

    GLfloat amb[] = {0,0,0,0};
    GLfloat diff[] = {1,1,1,1};
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);				// Setup The Ambient Light
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);				// Setup The Ambient Light
    glLightfv(GL_LIGHT1, GL_POSITION, lightpos);				// Setup The Ambient Light
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);				// Setup The Ambient Light
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);				// Setup The Ambient Light
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);				// Setup The Ambient Light

    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

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

