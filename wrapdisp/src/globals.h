#ifndef __HDR_globals_h
#define __HDR_globals_h


#include "includes.h"

struct tConfigurable
{
	int idx;
	string name;
};

struct tVideoMode:public tConfigurable
{
	int height,width,bpp;
};


extern int scr_width;
extern int scr_height;
extern int video_dofullscreen;
extern GLubyte pixels[1024*512*4];
extern uint32_t *pixw;

extern int dstart;
extern int dispmode;
extern int palstart;

extern uint8_t paldata[10000000];
extern int palsize;

extern int palentrysize;

extern uint8_t data[10000000];
extern int datalen;
extern int byteorder;
extern int bitsel;
extern int bitwidth;
extern int modified;

extern GLuint  thetex;
extern GLfloat scr_lrlim;
extern GLfloat scr_tblim;
extern string selectedsong;
extern int glfontaa;
extern GLint maxTexSize;
extern SDL_Surface* scrsurface;
extern int fullscreenmode;

extern mcolor C_WHITE;
extern mcolor C_BLACK;
extern mcolor C_TRANSPARENT;

extern SDL_Rect **videomodesdl;
extern const SDL_VideoInfo *origmode;
extern vstring videomodenames;

extern vector<tConfigurable*> videomodes;
extern tVideoMode *fsvideomode;

extern int width;

extern int gameover;
extern volatile int uu;


#endif
