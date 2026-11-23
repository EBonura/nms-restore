int save_iff(char *fname,char *source,char *pal,int width,int height);
#pragma aux save_iff parm caller [edx] [esi] [edi] [eax] [ebx];

		sprintf(filename,"PICT%04d.LBM",get_highest()+1);
		save_iff(filename,bak_screen,iff_palette,640,480);

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Get Highest Pict */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

int get_highest(void )
{
	int highest=0;
	int current=0;
	DIR *dirp;
	char numstring[5];
	struct dirent *direntp;

	dirp=opendir("PICT*.LBM");
	if(dirp==NULL)	printf("No files\n");
	else
	{
		while(1)
		{
			direntp=readdir(dirp);
			if(direntp==NULL) break;
			sprintf(filename,"%s",direntp->d_name);
			strncpy(numstring,filename+4,4);
			current=atoi(numstring);
			if(current>highest) highest=current;
		}
		closedir(dirp);
	}
	return highest;
}
