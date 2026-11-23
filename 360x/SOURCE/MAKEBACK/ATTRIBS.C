/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Phillip Trelford's Read Sprite Attrib Data */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include	"globals.h"

flag process_attrib_script(char *fname);

void load_attributes(void )
{
	process_attrib_script("attribs.s");
}

void strtolower(char *dest,char *source,long length)
{
	long i;
	char c;

	for(i=0;i<length;i++)
	{
		c=*source++;
		if(c>='A' && c<='Z') c+='a'-'A';
		*dest++=c;
	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Process Text Level Script */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
flag process_attrib_line(char *source,long length);

long current_sprite;

flag process_attrib_script(char *fname)
{
	long linestart,linelength;
	long i;
	char c;

	for(i=0;i<NAME_END;i++)
	{
		sprites_attribs[i].defined=NO;
		sprites_attribs[i].shootable=NO;
	}

	script=(char *) heap_load_file(fname);
	
	current_sprite=-1;

	lineno=0;
	i=0;
	linestart=0;
	linelength=0;

	while(i<fsize)
	{
		c=script[i++];
		if( c==13 || c==10 )
		{
			if( script[i]==13 || script[i]==10 ) i++;
//			if(linelength>0) strncpy(line,&script[linestart],linelength);
			if(linelength>0) strtolower(line,&script[linestart],linelength);
			line[linelength]=0;	
			//printf("Line %4i: %s\n",lineno,line);
			process_attrib_line(&script[linestart],linelength);
			lineno++;
			linestart=i;
			linelength=0;
		}
		else linelength++;
	}	

	heap_free_last();

	return TRUE;
}

enum {
  		ATTRIB_NAME,
  		ATTRIB_SHOOTABLE,
  		ATTRIB_AI,
  		ATTRIB_DEATH,
  		ATTRIB_SFX,
  		ATTRIB_ASTERIX,
  		ATTRIB_DONE,
		ATTRIB_END
		};


typedef struct
{
	char *string;
} strings;

strings attrib_names[ATTRIB_END]={
	"name:",
	"shootable:",
	"ai:",
	"death:",
	"sfx:",
	"*",
	"end"
	};

typedef struct
{
	long number;
	char *string;
} equate_list;

equate_list yes_or_nos[]={
	YES,"yes",
	NO,"no",
	TRUE,"true",
	FALSE,"false",
	-1,"null"
};


long process_equate_list(equate_list *equates,char *string)
{
	long i,n;

	i=0;
	do
	{
		n=equates[i].number;
		if(strcmp(message,equates[i].string)==0) break;
		i++;
	} while(n!=-1);
	return n;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Process Script Line */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

flag process_attrib_line(char *source,long length)
{
	long i,n;
	long index;
	long entry;
	long entry_length;
	char fname[8+1];

	entry=0;
	while(entry<ATTRIB_END)
	{
		entry_length=strlen(attrib_names[entry].string);	
		if(length>entry_length) 
		{
			if(strncmp(attrib_names[entry].string,source,entry_length)==0) break;
		}
		entry++;
	}
	if(entry==ATTRIB_END) return FAIL;
	
	length-=entry_length;
	index=entry_length;
	if( length<=0) return FAIL;

	switch (entry)
	{
		case ATTRIB_NAME:
			memset(&message,0,9);
			sscanf(&source[index],"%s",message);
			i=0;
			while(i<NAME_END)
			{						
				if(strnicmp(get_sprite_name(i),message,8)==0)
				{				
					current_sprite=i;
					sprites_attribs[i].defined=YES;
					printf("NAMED: %s\n",get_sprite_name(i));
					break;
				}
				i++;
			}
			if(i==NAME_END)
			{ 
				sprintf(message,"Unknown Sprite %s\n",fname);
				internal_message(message);
			}
			break;
		case ATTRIB_SHOOTABLE:
			if(current_sprite<0)
			{
				printf("Attribute Error: No sprite declared.\n");
				break;
			}
			sscanf(&source[index],"%s",message);
	
			n=process_equate_list(yes_or_nos,message);
			if(n==-1)
			{
				printf("Field \"%s\" unknown line: %i\n",message,lineno);
			}
			else
			{
				sprites_attribs[current_sprite].shootable=n;
				printf("Field \"%s\" value %i\n",message,n);
			}
			break;
		case ATTRIB_DEATH:
			if(current_sprite<0)
			{
				printf("Attribute Error: No sprite declared.\n");
				break;
			}
			sscanf(&source[index],"%s",message);
			printf("Death field %s\n",message);
			break;
		case ATTRIB_SFX:
			if(current_sprite<0)
			{
				printf("Attribute Error: No sprite declared.\n");
				break;
			}
			sscanf(&source[index],"%s",message);
			printf("SFX field %s\n",message);
			break;
	}

	return OK;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of file */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */


