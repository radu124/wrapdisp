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
  
#include "fileio.h"

#ifdef _WINDOWS

vector<string> listDirContents(string dir, int flags)
{
	WIN32_FIND_DATA ffd;
	HANDLE            hFind;
	
	vector<string> res;
	DIR *dp;
	struct dirent *dirp;
	//aedbg<<" scanning "<<dir<<" -pattern:"<<pattern<<endl;
	int i;
	for (i=0; i<dir.length(); i++)
		if (dir[i]=='/') dir[i]='\\';
	hFind = FindFirstFile((dir+"\\*.*").c_str(), &ffd);
	if (hFind == INVALID_HANDLE_VALUE) return res;
	do
	{
		int attr=ffd.dwFileAttributes;
		MESSAGE("lib: %s %d\n" ,ffd.cFileName ,(int) ffd.dwFileAttributes);
		if ((flags & 2) && !(flags & 1) && (attr & FILE_ATTRIBUTE_DIRECTORY)!=0) continue;
		if ((flags & 2) &&  (flags & 1) && (attr & FILE_ATTRIBUTE_DIRECTORY)==0) continue;		
		// hide hidden files by default
		if (!(flags & 4) && ffd.cFileName[0]=='.') continue;
		//if (boost::regex_match(dirp->d_name,re))
		MESSAGE("accepted\n");
		res.push_back(string(ffd.cFileName));
	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);
	
	return res;
}



#else

vector<string> listDirContents(string dir, int flags)
{
	vector<string> res;
	DIR *dp;
	struct dirent *dirp;
	//aedbg<<" scanning "<<dir<<" -pattern:"<<pattern<<endl;
	if((dp  = opendir(dir.c_str())) == NULL) return res;
	while ((dirp = readdir(dp)) != NULL) 
	{
		if ((flags & 2) && !(flags & 1) && dirp->d_type==DT_DIR) continue;
		if ((flags & 2) &&  (flags & 1) && dirp->d_type!=DT_DIR) continue;		
		// hide hidden files by default
		if (!(flags & 4) && dirp->d_name[0]=='.') continue;
		//if (boost::regex_match(dirp->d_name,re))
		res.push_back(string(dirp->d_name));
	}
	closedir(dp);
	return res;
}
#endif

int fileExists(std::string name)
{
	struct stat q;
	//aedbg<<"DIRCHECK";
        int err=stat(name.c_str(),&q);
	if (err) return 0;
	if (S_ISREG(q.st_mode)) return 1;
	return 0;
}

vector<char *> openfiledata;

int mapFile(string name, const char *&data, int &len)
{
	char *res;
	int i,x;
	x=-1;
	for (i=0; i<openfiledata.size(); i++) 
		if (openfiledata[i]==NULL) x=i;
	if (x<0)
	{
		x=openfiledata.size();
		openfiledata.push_back(NULL);
	}
	i=open(name.c_str(),O_RDONLY);
	if (i<0) return 0;
	len=lseek(i,0,SEEK_END);
	lseek(i,0,SEEK_SET);
	res=(char *) malloc(len+1);
	read(i,res,len);
	close(i);
	data=res;
	openfiledata[x]=res;
	return x+1;
}

void unmapFile(int i)
{
	if (openfiledata[i-1]==NULL) return;
	free(openfiledata[i-1]);
	openfiledata[i-1]=NULL;
}



