
/**************************************
*        ROUTINE DECLARATIONS         *
**************************************/

#include "360X.H"

/**************************************
*          DECLARE VARIABLES          *
**************************************/

jallocType JALLOC;

#ifdef X360_TEST_HEAP				/* RESTORE: TEST BUILDS ONLY (X360_CFLAGS=-DX360_TEST_HEAP): */
LONG	heappeak=0;				/* THE MOST OF THE BLOCK EVER IN USE, IN BYTES */
LONG	heapfail=0;				/* ALLOCATIONS THAT DID NOT FIT */
#endif

/**************************************
*   SHOW BLOCKS THAT ARE ALLOCATED    *
**************************************/

void 
ShowBlocks(jallocType *jp)
{

	int		l;
    uchar	*ucp;

	printf("\nJalloc block list...\n");
	printf("--------------------\n");
    printf("%d blocks:\n", jp->lastused+1);

	if (jp->lastused == -1) return;

	ucp = (uchar*)(jp->base+jp->size);
	for(l = 0; l <= jp->lastused; l++)
	{
		ucp -= jp->blocks[l].size;
		printf("    block #%d : at +%d (&%x), size %d status %d\n", l, jp->blocks[l].offset,(int)ucp, jp->blocks[l].size, jp->blocks[l].inuse);
	}

    printf("\n");

}

/**************************************
*           INITALISE JALLOC          *
**************************************/

void 
Jalloc_Init(jallocType *jp, ulong base, long size)
{

	int		l;

	jp->base = base;
	jp->size = size;

	Jalloc_Reset(jp);

	mallocused=0;

}

/**************************************
*       CLAIM BLOCK OF MEMORY         *
**************************************/

uchar
*Jalloc_Alloc(jallocType *jp, long size)
{
	int		l,k, oldsize;

	if (size%4 != 0)
    	size += 4-(size%4);			/* ROUND TO LONG BOUNDRY */

	if (size <= 0)					/* SILLY */
    	return NULL;

	if (jp->lastused == -1)			/* FIRST BLOCK EVER CLAIMED */
	{
		jp->blocks[0].inuse = 1;
		jp->blocks[0].size = size;
		jp->blocks[0].offset = 0;
		jp->lastused = 0;
        jp->noofactive = 1;
		return ((uchar*)(jp->base+jp->size-size));
	}

	for(l = 0; l < MAXJALLOCS; l++)
	{
		if ((jp->blocks[l].inuse == 0) && (jp->blocks[l].size >= size))
			break;
		if (l == jp->lastused+1)
			break; 
	}
	if (l == MAXJALLOCS)			/* COULD FIND NO AVAILABLE RECORD (SHOULDN'T HAPPEN) */
		return NULL;

	if (l == jp->lastused+1)			/* ADD TO THE END OF THE LIST */
	{
		oldsize = (jp->blocks[jp->lastused].offset+jp->blocks[jp->lastused].size);
#ifdef X360_TEST_HEAP
		if (jp->size-oldsize < size) heapfail++;
		else if (oldsize+size > heappeak) heappeak=oldsize+size;
#endif
		if (jp->size-oldsize < size)		/* NOT ENOUGH SPACE LEFT AT END OF BLOCK */
			return NULL;
		jp->lastused++;
		jp->blocks[jp->lastused].inuse = 1;
		jp->blocks[jp->lastused].size = size;
		jp->blocks[jp->lastused].offset = oldsize;
        jp->noofactive++;
		return ((uchar*)(jp->base+jp->size-size-jp->blocks[jp->lastused].offset));
	}

	oldsize = jp->blocks[l].size;
	jp->blocks[l].size = size;
	jp->blocks[l].inuse = 1;
	if ((size < oldsize) &&
		(jp->lastused < MAXJALLOCS-1))	/* IF THE OLD BLOCK WAS LARGER AND THERE IS ROOM TO GROW */
		{
		for(k = jp->lastused; k > l; k--)
			jp->blocks[k+1] = jp->blocks[k];
		k = l+1;
		jp->lastused++;        
		jp->blocks[k].inuse = 0;
		jp->blocks[k].size = oldsize-size;
		jp->blocks[k].offset = jp->blocks[l].offset+size; 
		}
	else if (size < oldsize)
    	jp->blocks[l].size = oldsize;		/* CAN'T SPLIT IT, SO TO AVOID DROPOUT */

	return (uchar*)(jp->base+jp->size-size-jp->blocks[l].offset);
}

/**************************************
*        FREE BLOCK OF MEMORY         *
**************************************/

void 
Jalloc_Free(jallocType *jp, uchar *blk)
{

	uchar	*tp;
    int		l, k;

	if (jp->lastused == -1)			/* NOTHING TO FREE */
		return;

	tp = (uchar*)(jp->base+jp->size);
	for(l = 0; l <= jp->lastused; l++)
	{
		tp -= jp->blocks[l].size;
		if (tp == blk)
			break;
	}

	if (l > jp->lastused)			/* COULD NOT FIND THE BLOCK TO ALLOCATE */
		return;

	if (l == jp->lastused)
   	{
		jp->lastused--;			/* LAST ITEM IN LIST */
		jp->noofactive--;
		if ((jp->lastused >= 0) &&
			(jp->blocks[jp->lastused].inuse == 0))	/* WAS THE PREVIOUS ONE EMPTY AS WELL */
			jp->lastused--;
		return;
	}

	jp->blocks[l].inuse = 0;
    jp->noofactive--;
	if (jp->blocks[l+1].inuse == 0)		/* MERGE WITH NEXT ONE */
	{
		jp->blocks[l].size += jp->blocks[l+1].size;
		for(k = l+1; k < jp->lastused; k++)
			jp->blocks[k] = jp->blocks[k+1];
        jp->blocks[jp->lastused--].inuse = 0;
	}
	if ((l > 0) && (jp->blocks[l-1].inuse == 0))	/* MERGE WITH PREVIOUS ONE */
	{
		jp->blocks[l-1].size += jp->blocks[l].size;
		for(k = l; k < jp->lastused; k++)
			jp->blocks[k] = jp->blocks[k+1];
		jp->blocks[jp->lastused--].inuse = 0;
	}
}

/**************************************
*        RESET JALLOC ROUTINE         *
**************************************/

void 
Jalloc_Reset(jallocType *jp)
{
	int		l;

	for(l = 0; l < MAXJALLOCS; l++)		/* INITIALISE THE BLOCK ARRAY */
	{
		jp->blocks[l].inuse = 0;
		jp->blocks[l].offset = 0;
		jp->blocks[l].size = 0;
	}

	jp->lastused = -1;
    jp->noofactive = 0;
}

