all:
	ccpsx -g -Xo$80080000 main.c -omain.cpe,main.sym
load:
	pqbload rcube.tmd 80010000
	pqbload rcube.tim 80020000
