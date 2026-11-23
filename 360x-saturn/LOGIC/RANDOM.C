#include <sgl.h>

/***********************************************************************/
//Returns an unsigned integer from 0 to 32767
int	rand(void)
{
	volatile	Uint32 random;
	volatile	FIXED	r;
					  
	r	=	slRandom();

	random	=	(Uint32)r;
	
	random &= 0x7fff;

	return((int)random);

}

/***********************************************************************/

