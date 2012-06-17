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

#include "includes.h"
#include "utils.h"
#include "font.h"
#include "globals.h"
#include "verbosity.h"
#include "video.h"
#include "scene.h"
#include "message.h"
#include "verbosity.h"

const char *mainfname=NULL;
const char *palfname=NULL;


#define ifargint(a,b) \
if (0==strcmp(argv[i],a)) \
{ \
	assume(i+1<argc,"expecting parameter value after %s\n",a); \
	b=atoi(argv[++i]); \
	continue; \
}


int main( int argc, char* argv[] )
{
	int i;
	for (i=1; i<argc; i++)
	{
		if (argv[i][0]=='-')
		{
			if (0==strcmp(argv[i],"-pal"))
			{
				assume(i+1<argc,"expecting parameter value after -pal\n");
				palfname=argv[++i];
				continue;
			}
			ifargint("-width",width);
			ifargint("-mode",dispmode);
			ifargint("-start",dstart);
			ifargint("-palstart",palstart);
			assume(0,"unknown parameter: %s\n",argv[i]);
		}
		mainfname=argv[i];
	}
	assume(mainfname,"Expecting one file argument");
	int fin;

	fin=open(mainfname,O_RDONLY);
	datalen=read(fin,data,10000000);
	close(fin);

	if (palfname)
	{
	fin=open(palfname,O_RDONLY);
	palsize=read(fin,paldata,10000000);
	close(fin);
	}

	MESSAGE("Data len is: %d\n",datalen);
	assume(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO), "Unable to initialize SDL: %s\n");
	findvideomodes();
	checkFullscreenRes();
	// SDL_GetError());
	init_fonts();
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	SDL_WM_SetCaption("Wrap-disp reverse engineering tool", "SDL");
	/* create window */
	scrsurface = SDL_SetVideoMode(
		scr_width,scr_height,
		 0, SDL_OPENGL | SDL_RESIZABLE |
		(video_dofullscreen?SDL_FULLSCREEN:0) );
	fullscreenmode = video_dofullscreen;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
	INFO(SPRITES,"Maximum texture size:%d\n", maxTexSize);
	glEnable( GL_TEXTURE_2D );
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SDL_EnableKeyRepeat(133,33);
	//glAlphaFunc(GL_GREATER,0.1);
	//glEnable(GL_ALPHA_TEST);

	initviewport();
	SDL_Event event;
	gameover = 0;
	genbig();
	/* message pump */
	while (!gameover)
	{
		/* look for an event */
		while (SDL_PollEvent(&event)) {
			/* an event was found */
			if (event.type==SDL_QUIT)
				gameover=1;
			else if (event.type==SDL_VIDEORESIZE)
			{
				doResize(event.resize.w,event.resize.h);
			}
			else
				scenehandleevent(event);

			switch (event.type) {
				/* close button clicked */
				case SDL_QUIT:
					gameover = 1;
					break;
			}
		}

		renderscene();
		glFlush();
		glFinish();
		usleep(50000);
		SDL_GL_SwapBuffers();
	}

	/* free the background surface */
	//SDL_FreeSurface(bg);

	/* cleanup SDL */
	SDL_Quit();
	return 0;
}




