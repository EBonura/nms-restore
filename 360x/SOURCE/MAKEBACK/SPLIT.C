char 	*split_buf;
int   ships_fuel;
int   ships_shield;
int   ships_fuel_deplete;
int   ships_fuel_add;
int   ships_shield_add;
int	ships_gravity;

/* ßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßßß */
/* Split Print */
/* ÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜÜ */

void update_fuel(void)
{
	int i;
	if(ships_fuel_add>0 && ships_fuel<(640-(4+36*8)-11))
	{
		ships_fuel_add--;
		for(i=0;i<9;i++)
		{
			screen_dump_line(split_buf+7*640+1,4+36*8+640*11+ships_fuel+i*640,1);
		}
		ships_fuel++;
	}

	if(ships_fuel_add<0)
	{
		ships_fuel_add++;
		ships_fuel--;
		for(i=0;i<9;i++)
		{
			screen_dump_line(split_buf+7*640+0,4+36*8+640*11+ships_fuel+i*640,1);
		}
	}

	if(ships_shield_add>0 && ships_shield<(640-(4+36*8)-11))
	{
		ships_shield_add--;
		for(i=0;i<9;i++)
		{
			screen_dump_line(split_buf+23*640+1,4+36*8+640*43+ships_shield+i*640,1);
		}
		ships_shield++;
	}

	if(ships_shield_add<0)
	{
		ships_shield_add++;
		ships_shield--;
		for(i=0;i<9;i++)
		{
			screen_dump_line(split_buf+23*640+0,4+36*8+640*43+ships_shield+i*640,1);
		}
	}
}

void print_string(char *string,int x,int y)
{
	char c;
	while((c=(*string++))!=0)
		print_char(c,x++,y);
}

void print_char(char c,int x,int y)
{
	int iy,source=10*8*640+16+38*8,dest;
	
	if(c>='a' & c<='z') c+=('A'-'a');
	if(c>='A' & c<='Z') source=10*8*640+16+(c-'A')*8;
	if(c>='0' & c<='9') source=10*8*640+16+26*8+(c-'0')*8;
	if(c=='-') source=10*8*640+16+36*8;
	if(c=='.') source=10*8*640+16+37*8;
	dest=x*8+y*8*640;
	for(iy=0;iy<8;iy++)
		screen_dump_line(split_buf+source+iy*640,dest+iy*640,8);
}


