short sin_table[256];

for(i=0;i<256;i++)
{
	sin_table[i]=sin((i*pi*2)/256)*32767;
}
