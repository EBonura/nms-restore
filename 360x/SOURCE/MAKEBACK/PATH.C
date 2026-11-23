/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Sprites Path's Code */ 
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

#include "globals.h"

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Get Next Path Line */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */
// Updates DX & DY
// Moves to the next node...
// Doesn't handle Mark's -2s
// Doesn't handle different directions on paths

void get_path_coordinates(long *node,long *dx,long *dy)
{
	long	temp;
	char *source;
	long *sourcel;

	source=path_data;
	sourcel=(long * ) path_data;

	temp=sourcel[1];
	source+=temp;
	sourcel=(long * ) source;
	sourcel+=(*node)*10;

	sourcel++;
	sourcel++;
	*dx=*sourcel++;
	*dy=*sourcel++;
	sourcel++;
	*node=*sourcel++;
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* Debug Path Data */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */

void view_path_data(void)
{
	long	l;
	char *source;
	short *sourcew;
	long points;
	long *sourcel;
	long *path_datal;
	long *parentl;
	long temp;
	long i,j,k;
	long line_type;
	long sprite_parent;
	long x,y;
	long dx,dy;
	long parent;
	long child[4];
	long flags;

	source=path_data;
	sourcew=(short * ) path_data;
	sourcel=(long * ) path_data;

	points=(long )sourcew[1];
	printf("Number of Points %i\n",points);

	temp=sourcel[1];
	printf("Source Offset %i\n",temp);
	source+=temp;
	sourcel=(long * ) source;
	path_datal=sourcel;

	l=1;

	for(i=0;i<points;i++)
	{
		line_type=*sourcel++;			//0
		sprite_parent=*sourcel++;		//1
		dx=*sourcel++;						//2
		dy=*sourcel++;						//3
		parent=*sourcel++;				//4
		child[0]=*sourcel++;				//5
		child[1]=*sourcel++;				//6
		child[2]=*sourcel++;				//7
		child[3]=*sourcel++;				//8
		flags=*sourcel++;					//9

		temp=0;
		if(child[0]==0) temp++;
		if(child[1]==0) temp++;
		if(child[2]==0) temp++;
		if(child[3]==0) temp++;
		if(temp==4) printf("Point %i Invalid\n",i);

		k=0;
		if(child[0]==i) k++;
		if(child[1]==i) k++;
		if(child[2]==i) k++;
		if(child[3]==i) k++;

		if(k>0)
		{ 
			printf("Point %i's child is itself\n",i);
			temp=4;
		}	

		if(temp!=4)
		{
			printf("Point %i:- X:%i,Y:%i,Parent %i,Sp_Parent %i\n",i,dx,dy,parent,sprite_parent);
			printf("Child %i,%i,%i,%i\n",child[0],child[1],child[2],child[3]);
	
			for(j=0;j<MAX_SPRITES;j++)
			{
				if(sprites[j].map_index==sprite_parent)
				{
					x=sprites[j].x;
					y=sprites[j].y;
					printf("%s %i,%i\n",get_sprite_name(sprites[j].name),x,y );
					break;
				}
			}

			for(j=0;j<4;j++)
			{
				if(child[j]==-2)
				{
					lines[l].line_on=1024;
					lines[l].x1[0]=x+dx;
					lines[l].x1[1]=x+dx;
					lines[l].y1[0]=y+dy;
					lines[l].y1[1]=y+dy;

					lines[l].x2[0]=x;
					lines[l].x2[1]=x;
					lines[l].y2[0]=y;
					lines[l].y2[1]=y;

					l++;
				}
			}

			if( parent!=-1)
			{

				lines[l].x1[0]=x+dx;
				lines[l].x1[1]=x+dx;
				lines[l].y1[0]=y+dy;
				lines[l].y1[1]=y+dy;
				
				temp=parent;
				parentl=path_datal+parent*10;
		
				line_type=*parentl++;			//0
				sprite_parent=*parentl++;		//1
				dx=*parentl++;						//2
				dy=*parentl++;						//3
				parent=*parentl++;				//4
				child[0]=*parentl++;				//5
				child[1]=*parentl++;				//6
				child[2]=*parentl++;				//7
				child[3]=*parentl++;				//8
				flags=*parentl++;					//9
				printf("Parent X:%i,Y:%i\n",dx,dy);
				k=0;
				for(j=0;j<4;j++)
				{
					if(i==child[j]) k++;
				}
				if(k==0)
				{
					printf("Point %i's parent does not have it listed as a child\n",i);
				}
				else
				{
					lines[l].x2[0]=x+dx;
					lines[l].x2[1]=x+dx;
					lines[l].y2[0]=y+dy;
					lines[l].y2[1]=y+dy;
					lines[l].line_on=1024;
					l++;
				}
			}
			else
			{
				lines[l].line_on=1024;
				lines[l].x1[0]=x+dx;
				lines[l].x1[1]=x+dx;
				lines[l].y1[0]=y+dy;
				lines[l].y1[1]=y+dy;
		
				lines[l].x2[0]=x;
				lines[l].x2[1]=x;
				lines[l].y2[0]=y;
				lines[l].y2[1]=y;
				l++;
			}

			if(l>250) return;
		}

	}
}

/* ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ */
/* End of File */
/* ‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹ */


