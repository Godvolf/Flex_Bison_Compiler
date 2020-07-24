%option yylineno
%{
// %option noyywrap
// #define YYSTYPE string
#include <stdio.h>
#include <string.h>
#include "bison.tab.h"


extern void yyerror (char const *);
int kom = 0;
%}

%%
\[ { if (!kom) { kom = 1; } else printf("error comments"); }
\] { kom = 0; }

DECLARE { if (!kom) { return DECLARE; } }
BEGIN { if (!kom) return BGN; }
END { if (!kom) return END; }

ASSIGN { if (!kom) return ASSIGN; }

IF { if (!kom) return IF; }
THEN { if (!kom) return THEN; }
ELSE { if (!kom) return ELSE; }
ENDIF { if (!kom) return ENDIF; }

WHILE { if (!kom) return WHILE; }
DO { if (!kom) return DO; }
ENDWHILE { if (!kom) return ENDWHILE; }
ENDDO { if (!kom) return ENDDO; }

FOR { if (!kom) return FOR; }
FROM { if (!kom) return FROM; }
TO { if (!kom) return TO; } 
DOWNTO { if (!kom) return DOWNTO; }
ENDFOR { if (!kom) return ENDFOR; }

READ { if (!kom) return READ; }
WRITE { if (!kom) return WRITE; }

PLUS { if (!kom) return PLUS; }
MINUS { if (!kom) return MINUS; }
TIMES { if (!kom) return TIMES; }
DIV { if (!kom) return DIV; }
MOD { if (!kom) return MOD; }

EQ { if (!kom) return EQ; }
NEQ { if (!kom) return NEQ; }
LE { if (!kom) return LE; }
GE { if (!kom) return GE; }
LEQ { if (!kom) return LEQ; }
GEQ { if (!kom) return GEQ; }


[_a-z]+ { if (!kom) { yylval.str = (char *)strdup(yytext); return IDE; }}
-?[0-9]+ { if (!kom) { yylval.str = (char *)strdup(yytext); return NUM; }}

":" { if (!kom) return COL;}
";" { if (!kom) return SEM;}
"," { if (!kom) return COM;}
"(" { if (!kom) return LB;}
")" { if (!kom) return RB;}

[ \t\r\n]+
.
%%
int yywrap(){return 1;}

