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
#include "scene.h"
#include "globals.h"
#include "message.h"

void genbig()
{
	glGenTextures(1, &thetex);
	glBindTexture(GL_TEXTURE_2D, thetex);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	updatetexture();
}

void renderscene()
{
	updatetexture();
	glBindTexture(GL_TEXTURE_2D,thetex);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0.0 , 0.0); glVertex3f(0      ,    0, 0);
	glTexCoord2f(1.0, 0.0); glVertex3f(scr_width,0, 0);
	glTexCoord2f(0.0 , 1.0); glVertex3f(0, scr_height, 0);
	glTexCoord2f(1.0 , 1.0); glVertex3f(scr_width, scr_height, 0);
	glEnd();
}

void setpel(int i, int r, int g=-1, int b=-1)
{
	r=r&255;
	if (g<0) g=r;
	if (b<0) b=r;
	pixels[i*3]=r;
	pixels[i*3+1]=g;
	pixels[i*3+2]=b;
}

void setpelw(int i, uint32_t rgb)
{
	pixels[i*3  ]=(uint8_t)  rgb;
	pixels[i*3+1]=(uint8_t) (rgb>>8);
	pixels[i*3+2]=(uint8_t) (rgb>>16);
}

int getcrtwordsize()
{
	switch (dispmode)
	{
	case 0:
		return 1;
	case 1:
	case 2:
		return 2;
	}
	return 1;
}

uint32_t getpixelval(uint8_t *data)
{
	uint32_t val=0;
	int mask;
	int q=data[0];
	switch (dispmode)
	{
	case 0:
		if (palsize==0) val=((q&0x07)*36) | ((q&0x38)/8*36)*256 | ((q&0xc0)/0x40*85)*0x10000;
		else
		{
			val  = paldata[q*palentrysize+palstart]*0x10000;
			val += paldata[q*palentrysize+palstart+1]*256;
			val += paldata[q*palentrysize+palstart+2];
		}
		return val;
	case 1:
		if (byteorder) val=data[1]+256*data[0];
		else val=data[0]+256*data[1];
		mask=(1<<bitwidth)-1;
		val=(val*0x10001) & (mask<<bitsel);
		val=val>>bitsel;
		val=val<<(8-bitwidth);
		val=val&0xff;
		return val;
	case 2:
		if (byteorder) val=data[1]+256*data[0];
		else val=data[0]+256*data[1];
		val=val & 0xffff;
		val=((val>>11)&0x1f)*8 | 256*((val>>5) &0x3f)*4 | 0x10000*((val) & 0x1f)*8;
		break;
	}
	return val;
}

void updwords()
{
	int i, j, k;
	int wordsize=getcrtwordsize();
	k=0;
	memset(pixels,0,sizeof(pixels));
	int adr=dstart;
	while (k<1024)
	{
		for (i=0; i<512; i++)
		{
			for (j=0; j<width/wordsize; j++)
			{
				if (j+k<1024) setpelw(i*1024+j+k,getpixelval(data+adr));
				adr+=wordsize;
			}
			if (adr>=datalen) break;
		}
		if (adr>=datalen) break;
		k+=width/wordsize+10;
	}
}


void updatetexture()
{
	if (modified) updwords();
	//for (i=0; i<1024*512; i++) setpel(i,i,0,0);
	glBindTexture(GL_TEXTURE_2D, thetex);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 1024, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
}

int modif=0;

void dumpdata()
{
	int i,j,adr;
	FILE *fou=fopen("dump.txt","w");
	adr=dstart;
	fprintf(fou,"%d = %08x:\n",adr,adr);
	for (i=0; i<512; i++)
	{
		for (j=0; j<width; j++)
		{
			fprintf(fou," %02x"+(j==0),data[adr]);
			adr++;
		}
		fprintf(fou,"\n");
		if (adr>datalen) break;
	}
	fclose(fou);
}

void scenehandleevent(SDL_Event &event)
{
	int wordwidth=1;
	if (dispmode==1 || dispmode==2) wordwidth=2;

	if (event.type==SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_LCTRL:
		case SDLK_RCTRL: modif|=1;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT: modif|=2;
		}
	}
	if (event.type==SDL_KEYUP)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_RCTRL:
		case SDLK_LCTRL: modif&=~1;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT: modif&=~2;
		}
	}
	modified=0;
	if (event.type==SDL_KEYDOWN)
	{
		modified=1;
		switch (event.key.keysym.sym)
		{
		case SDLK_PAGEUP:
			dstart-=width*512;
			if (dstart<0) dstart=0;
			break;
		case SDLK_PAGEDOWN:
			dstart+=width*512;
			if (dstart>datalen) dstart=datalen;
			break;
		case SDLK_UP:
			dstart-=(modif&1)?width:wordwidth;
			if (dstart<0) dstart=0;
			break;
		case SDLK_DOWN:
			dstart+=(modif&1)?width:wordwidth;
			if (dstart>datalen) dstart=datalen;
			break;
		case SDLK_RIGHT:
			if (width<10000)
			width+=wordwidth*((modif & 1)?32:1);
			break;
		case SDLK_LEFT:
			width-=wordwidth*((modif & 1)?32:1);
			if (width<1) width=1;
			break;
		case SDLK_SPACE:
			dispmode=(dispmode+1)%3;
			break;
		case SDLK_ESCAPE: break;
		case SDLK_RETURN:
			break;
		case 'q': bitsel=(bitsel+1) & 15; break;
		case 'a': bitsel=(bitsel-1) & 15; break;
		case '1': bitwidth=1; break;
		case '2': bitwidth=2; break;
		case '3': bitwidth=3; break;
		case '4': bitwidth=4; break;
		case '5': bitwidth=5; break;
		case '6': bitwidth=6; break;
		case '7': bitwidth=7; break;
		case '8': bitwidth=8; break;
		case 's':
			byteorder^=1;
			break;
		case 'd':
			dumpdata();
			break;
		case '[': if (palstart>0) palstart--; break;
		case ']': if (palstart<palsize-3) palstart++; break;
		}
		MESSAGE("Width: %6d   Start: %8d   Mode: %1d   bitpos: %2d   bitwidth: %2d palstart: %d\n",width,dstart,dispmode,bitsel,bitwidth,palstart);

	}
}



