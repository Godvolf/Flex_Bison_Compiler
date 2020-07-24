program: bison.y flex.l
	rm -f lex.yy.c bison.tab.c bison.tab.h output.mr
	bison -d -v bison.y 
	lex flex.l 
	gcc -o kompilator lex.yy.c bison.tab.c -ll -lm
