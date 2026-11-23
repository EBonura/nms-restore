
all: tuto1
	
tuto1:
	ccpsx -O -Xo$80080000 tuto1.c -otuto1.cpe,tuto1.sym
load:
	pqbload mado.tim 80180000
	pqbload mado.tmd 80040000

	

