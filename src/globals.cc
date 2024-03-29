/*******************************************************************
(C) 2011 by Radu Stefan
radu124@gmail.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*******************************************************************/

#include "globals.h"

int scr_width=1024;
int scr_height=512;
int video_dofullscreen=0;
GLubyte pixels[1024*512*4];
uint32_t *pixw=(uint32_t *) &pixels[0];

int dstart=0;
int dispmode=0;
int palstart=0;

uint8_t paldata[10000000];
int palsize=0;

int palentrysize=4;

uint8_t data[10000000];
int datalen;
int byteorder=0;
int bitsel=0;
int bitwidth=8;
int modified=1;
// gave up namespaces
GLuint  thetex;
GLfloat scr_lrlim;
GLfloat scr_tblim;
string selectedsong="";
int glfontaa=GL_LINEAR;
GLint maxTexSize;
SDL_Surface* scrsurface;
int fullscreenmode=0;

mcolor C_WHITE(1,1,1,1);
mcolor C_BLACK(0,0,0,1);
mcolor C_TRANSPARENT(0,0,0,1);

SDL_Rect **videomodesdl;
const SDL_VideoInfo *origmode;
vstring videomodenames;

vector<tConfigurable*> videomodes;
tVideoMode *fsvideomode;

int width=32;

int gameover;
volatile int uu;

