/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "bison.y" /* yacc.c:339  */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h> 
int yylex (void);
void yyerror (const char *);

// ---------------------------------------------------------- Początkowe zmienne ------------------------------------------------------

long long memIdx = 10; //wskazuje ostatnią wolną komórkę pamięci, zaczynając od 10
long long lines = 1;
long long labelCounter = 0;

// ----------------------------------------------------------- Definicja struktur ------------------------------------------------------

// Tablica symboli - lista jednokierunkowa
struct symTable 
{
	long long index;
	char *name;
	struct symTable *next;
};


// Tablica symboli dla tablic - lista jednokierunkowa
struct symArray
{
	long long startMemIdx;
	long long startTabIdx;
	long long endTabIdx;
	long long len;
	char *tabName;
	struct symArray *next;
};


// Lista komend maszyny wirtualnej
struct commands  {
	char *command;
	bool isNumber;
	long long number;
	struct commands *next;
	struct commands *prev;
};

// Stos aktualnie używanych zmiennych
struct ideStack {
	int isNumber;	
	long long value; // for number
	long long memIdx; // for variable or tab(number)
	long long tabMemIdx; // startowa komórka w pamięci dla tabeli
	long long tabIdx; // początkowy indeks tabeli
	long long poczatek;
	bool result;
	long long label;
	int isValue;
	struct ideStack* next;
};


// ------------------------------------------------------ Inicjowanie struktur -----------------------------------------
struct symTable *head = NULL;
struct symArray *arrHead = NULL;

struct commands *first = NULL;
struct commands *last = NULL;
struct commands *currentComm = NULL;

struct ideStack *root = NULL; 

// --------------------------------------------------------------------------- Komendy -----------------------------------------------------


// metody do generowania listy komend maszyny wirtualnej
void insertCommand(char *command) 
{
	char t;
	for (int i=0; command[i]!=0; i++) {
		if(command[i]=='\n') {
			lines++;
		}
	}
	struct commands *link = (struct commands*) malloc(sizeof(struct commands));
	link->command = command;
	link->isNumber = false;
	link->prev = NULL;
	link->next = NULL;
	if(first==NULL) 
	{
	    first = link;
	    return;
	}
	currentComm = first;
	while(currentComm->next!=NULL)
	   currentComm = currentComm->next;
	   currentComm->next = link;
	   last = link;
	   link->prev = currentComm;
}


void insertCommandNum(long long number) 
{
	struct commands *link = (struct commands*) malloc(sizeof(struct commands));
	link->number = number;
	link->isNumber = true;
	link->prev = NULL;
	link->next = NULL;
	if(first==NULL) 
	{
	    first = link;
	    return;
	}
	currentComm = first;
	while(currentComm->next!=NULL)
	   currentComm = currentComm->next;
	   currentComm->next = link;
	   last = link;
	   link->prev = currentComm;
}


void printCommandsToFile(FILE *fp) 
{
	if (labelCounter>0) {
		long long label = 1;
		bool hashFlag = false;
		char t;
		long long hashVal;
		long long lineHash;
		while (label <= labelCounter) {
			// znajdz ostatnie wystapienie
			struct commands *ptr = first;
			while(ptr->next != NULL) {
				hashFlag = false;
				if(ptr->isNumber == false) {
					for (int i=0; ptr->command[i]!=0; i++) {
						if(ptr->command[i]=='#') {
							hashFlag = true;
						}
					}
					if (hashFlag == true) {
						ptr = ptr->next;
						hashVal = ptr->number;
						if(hashVal == label) {
							ptr = ptr->next;
							if (ptr->command[0] == ' ') {
								ptr = ptr->next;
								lineHash = ptr->number;
								break;
							}
						}
					}
				}
				ptr = ptr->next;
			}

			ptr = first;
			while(ptr->next != NULL) {
				if(ptr->isNumber == false) {
					for (int i=0; ptr->command[i]!=0; i++) {
						if(ptr->command[i]=='#') {
							if(ptr->next->number == label) {
								if(ptr->next->next->command[0] == ' ') {
									ptr->prev->next = ptr->next->next->next->next->next;
									ptr = ptr->prev;
								} else {
									ptr->prev->next = ptr->next;
									ptr->next->number = lineHash-1;
									ptr = ptr->prev;
								}
							} 
						}
					}
				}
				ptr = ptr->next;
			}
			label ++;
		}
	}

	struct commands *ptr = first;
	while(ptr->next != NULL) 
	{
		if(ptr->isNumber == true) 
		{
			fprintf(fp, "%lld", ptr->number);
		} else {
			fprintf(fp, "%s", ptr->command);
		}
		ptr = ptr->next;
	}
	if(ptr->isNumber == true) 
	{
		fprintf(fp, "%lld", ptr->number);
	} else {
		fprintf(fp, "%s", ptr->command);
	}
	fclose(fp);
}

//	------------------------------------------------------- WSTAWIANIE --------------------------------------------------

// Wstaw zmienną o podanej nazwie do symTable
void insertVariable(char* name) 
{
	struct symTable *link = (struct symTable*) malloc(sizeof(struct symTable));
	link->index = memIdx;
	link->name = name;
	link->next = head;
	head = link;
	memIdx++;
}

void insertArray(char* startIndex, char* endIndex, char* name) 
{
	long long start = atoll(startIndex);
	long long end = atoll(endIndex);
	long long len = end-start+1;
	struct symArray *link = (struct symArray*) malloc(sizeof(struct symArray));
	link->startMemIdx = memIdx;
	link->startTabIdx = start;
	link->endTabIdx = end;
	link->len = len;
	link->tabName = name;
	link->next = arrHead;
	arrHead = link;

	memIdx += len + 1;

}

// --------------------------------------------------------------------------- Wyszukiwanie -----------------------------------------------------

// Znajdź zmienną o podanej nazwie w tablicy symboli, zwróć jej indeks w pamięci
long long findVariable(char* name)
{
     struct symTable *ptr = head;
     while(ptr != NULL) 
     {
          if (strcmp(ptr->name, name) == 0) {
	    return ptr->index; //znaleziono
	  }
          ptr = ptr->next;
     }
     return -1; // nie znaleziono
}

// Znajdź zmienną o podanym indeksie w pamięci, zwróć wskaźnik do niej
struct symTable* findVariableByIndex(long long index)
{
	struct symTable *ptr = head;
	while(ptr != NULL) 
	{
		if (ptr->index == index) {
			return ptr; //znaleziono
		}
		ptr = ptr->next;
	}
	return NULL; // nie znaleziono
}

long long findArrayIndex(char* name, long long tabIdx) 
{
	struct symArray *ptr = arrHead;
	while (ptr != NULL) {
		if ((strcmp(ptr->tabName, name) == 0)) {
			return ((tabIdx - ptr->startTabIdx) + ptr->startMemIdx);
		}
		ptr = ptr->next;
	}
	return -1; // nie znaleziono
}

struct symArray* findArray(char* name)
{
	struct symArray *ptr = arrHead;
	while (ptr != NULL) {
		if ((strcmp(ptr->tabName, name) == 0)) {
			return ptr;
		}
		ptr = ptr->next;
	}
	return NULL; // nie znaleziono
}
// ------------------------------------------------------------------------------------- STOS --------------------------------------------

// Metody dla stosu używanych zmiennych
struct ideStack* newIdeNum(long long value) 
{ 
	struct ideStack* newIden = (struct ideStack*)malloc(sizeof(struct ideStack)); 
	newIden->isNumber = 1; 
	newIden->value = value;
	newIden->next = NULL; 
	return newIden; 
} 

struct ideStack* newIdeVar(long long memIdx) 
{ 
	struct ideStack* newIden = (struct ideStack*)malloc(sizeof(struct ideStack)); 
	newIden->isNumber = 0; 
	newIden->memIdx = memIdx;
	newIden->next = NULL; 
	return newIden; 
} 

struct ideStack* newIdeIdeTab(long long memoIdx, char* tabName)
{
	struct ideStack* newIden = (struct ideStack*)malloc(sizeof(struct ideStack)); 
	struct symArray* arr = findArray(tabName);
	newIden->isNumber = 2;
	newIden->memIdx = memoIdx;
	newIden->tabMemIdx = arr->startMemIdx;
	newIden->tabIdx = arr->startTabIdx;
	newIden->poczatek = 0;
	newIden->next = NULL; 
}

struct ideStack* newExprResult(long long label)
{
	struct ideStack* newIden = (struct ideStack*)malloc(sizeof(struct ideStack)); 
	newIden->label = label;
	newIden->next = NULL;
	return newIden; 

}
  
int isEmpty(struct ideStack* root) 
{ 
    return !root; 
} 
  
void push(struct ideStack** root, int isNumber, long long data) 
{ 
	struct ideStack* newIden;
	if (isNumber) {
		newIden = newIdeNum(data);
	} else {
		newIden = newIdeVar(data);
	}
	newIden->next = *root;
	*root = newIden;
} 

void pushVal(struct ideStack** root, int isNumber, long long data, int IsValue) {
	struct ideStack* newIden;
	newIden = newIdeVar(data);
	newIden->isValue = 1;
	newIden->next = *root;
	*root = newIden;
}

void pushIdeIde(struct ideStack** root, int isNumber, long long memoIdx, char* tabName) 
{ 
	struct ideStack* newIden;
	newIden = newIdeIdeTab(memoIdx, tabName);
	newIden->next = *root;
	*root = newIden;
} 

void pushExpr(struct ideStack** root,long long label) {
	struct ideStack* newIden;
	newIden = newExprResult(label);
	newIden->isValue = 4;
	newIden->next = *root;
	*root = newIden;
}
  
long long popNum(struct ideStack** root) 
{ 
	if (isEmpty(*root)) {
		return INT_MIN; 
	}
	struct ideStack* temp = *root; 
	*root = (*root)->next; 
	long long value = temp->value; 
	free(temp); 
  
    return value; 
}

long long popVar(struct ideStack** root) 
{ 
	if (isEmpty(*root)) {
		return INT_MIN; 
	}
	struct ideStack* temp = *root; 
	*root = (*root)->next; 

	long long value = temp->memIdx; 
	free(temp); 

	return value; 
} 

struct ideStack* popIdeIde(struct ideStack** root)
{
	if (isEmpty(*root)) {
		return NULL; 
	}
	struct ideStack* temp = *root; 
	*root = (*root)->next; 

	return temp; 
}

int peekIsNumber(struct ideStack* root) 
{ 
     if (isEmpty(root)) 
         return INT_MIN; 
     return root->isNumber; 
} 

// ------------------------------------------------------ METODY ------------------------------------------------------------------------------

// generowanie stałej
void genConst(long long number) 
{
	if(number == 0) {
		insertCommand("SUB 0\n");
	} else if(number > 0) {
		insertCommand("SUB 0\n");
		insertCommand("STORE 3");
		// insertCommandNum(memIdx+5);
		insertCommand("\n");

		
		long long actualValue;
		long long wykladnik;

		while (number > 100) {
			actualValue = 1;
			wykladnik = 0;
			while(actualValue*2 <= number) {
				actualValue = actualValue*2;
				wykladnik ++;
			}
			number -= actualValue;
			for (int i = 0; i<wykladnik; i++) {
				insertCommand("INC\n");
			}
			insertCommand("STORE 2\n");
			insertCommand("SUB 0\n");
			insertCommand("INC\n");
			insertCommand("SHIFT 2\n");
			insertCommand("ADD 3");
			// insertCommandNum(memIdx+5);
			insertCommand("\n");
			insertCommand("STORE 3");
			// insertCommandNum(memIdx+5);
			insertCommand("\n");
		}
		for(int i = 0; i<number; i++) {
			insertCommand("INC\n");
		}
	} else {
		insertCommand("SUB 0\n");
		insertCommand("STORE 3");
		// insertCommandNum(memIdx+5);
		insertCommand("\n");
		long long actualValue;
		long long wykladnik;

		while (number < -100) {
			actualValue = -1;
			wykladnik = 0;
			while(actualValue*2 >= number) {
				actualValue = actualValue*2;
				wykladnik ++;
			}
			number -= actualValue;
			for (int i = 0; i<wykladnik; i++) {
				insertCommand("INC\n");
			}
			insertCommand("STORE 2\n");
			insertCommand("SUB 0\n");
			insertCommand("DEC\n");
			insertCommand("SHIFT 2\n");
			insertCommand("ADD 3");
			// insertCommandNum(memIdx+5);
			insertCommand("\n");
			insertCommand("STORE 3");
			// insertCommandNum(memIdx+5);
			insertCommand("\n");
			insertCommand("SUB 0\n");
		}
		for(int i = 0; i<abs(number); i++) {
			insertCommand("DEC\n");
		}
		insertCommand("ADD 3");
		// insertCommandNum(memIdx+5);
		insertCommand("\n");
	}
}

void ifFun(){
	struct ideStack* iS = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iS->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
}

void elseFun() {
	struct ideStack* iS = popIdeIde(&root);
	labelCounter++;
	insertCommand("JUMP ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);
	insertCommand("#");
	insertCommandNum(iS->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
}

void startWhile() {
	labelCounter++;
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	pushExpr(&root, labelCounter);
}

void whileFun() {
	struct ideStack* iSMark = popIdeIde(&root);
	struct ideStack* iSJump = popIdeIde(&root);

	insertCommand("JUMP ");
	insertCommand("#");
	insertCommandNum(iSJump->label);
	insertCommand("\n");

	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");

}

void forFrom(char *name) {
	long long nameIdx = findVariable(name);
	if (nameIdx == -1) {
		// nie znaleziono - zadeklaruj nową
		insertVariable(name);
		nameIdx = findVariable(name);
	}
	// dodatkowe miejsce dla zmiennej po TO
	pushVal(&root, 0, nameIdx, 1);
	pushVal(&root, 0, nameIdx, 1);
	memIdx += 2;
}

// Przechowaj wartość po FROM w zmiennej
void forTo() {
	int peekNumberVal = peekIsNumber(root);
	if (peekNumberVal == 1) { 
		long long val =  popNum(&root);
		long long variableIdx = popVar(&root);
		genConst(val);
		insertCommand("STORE ");
		insertCommandNum(variableIdx);
		insertCommand("\n");
	} else if (peekNumberVal == 0) {
		long long val = popVar(&root);
		long long variableIdx = popVar(&root);
		insertCommand("SUB 0\n");
		insertCommand("LOAD ");
		insertCommandNum(val);
		insertCommand("\nSTORE ");
		insertCommandNum(variableIdx);
		insertCommand("\nSUB 0\n");	
	} else {
		struct ideStack *iS = popIdeIde(&root);
		long long variableIdx = popVar(&root);
		long long wyr = iS->tabMemIdx - iS->tabIdx;
		genConst(wyr);
		insertCommand("ADD ");
		insertCommandNum(iS->memIdx);
		insertCommand("\nLOADI 0");
		insertCommand("\nSTORE ");
		insertCommandNum(variableIdx);
		insertCommand("\nSUB 0\n");
	}
}

// Przechowaj wartość po TO w memo (zmienna+1)
void forDo() {
	int peekNumberVal = peekIsNumber(root);
	long long variableIdx;
	if (peekNumberVal == 1) { 
		long long val =  popNum(&root);
		variableIdx = popVar(&root);
		genConst(val);
		insertCommand("STORE ");
		insertCommandNum(variableIdx+1);
		insertCommand("\n");
	} else if (peekNumberVal == 0) {
		long long val = popVar(&root);
		variableIdx = popVar(&root);
		insertCommand("SUB 0\n");
		insertCommand("LOAD ");
		insertCommandNum(val);
		insertCommand("\nSTORE ");
		insertCommandNum(variableIdx+1);
		insertCommand("\nSUB 0\n");	
	} else {
		struct ideStack *iS = popIdeIde(&root);
		variableIdx = popVar(&root);
		long long wyr = iS->tabMemIdx - iS->tabIdx;
		genConst(wyr);
		insertCommand("ADD ");
		insertCommandNum(iS->memIdx);
		insertCommand("\nLOADI 0");
		insertCommand("\nSTORE ");
		insertCommandNum(variableIdx+1);
		insertCommand("\nSUB 0\n");
	}

	labelCounter++;
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	pushExpr(&root, labelCounter);
 
	insertCommand("SUB 0\n");
	insertCommand("LOAD ");
	insertCommandNum(variableIdx);
	insertCommand("\n");
	insertCommand("SUB ");
	insertCommandNum(variableIdx+1);
	insertCommand("\n");

	labelCounter++;
	insertCommand("JPOS ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);
}

void forDoDownto() {
	int peekNumberVal = peekIsNumber(root);
	long long variableIdx;
	if (peekNumberVal == 1) { 
		long long val =  popNum(&root);
		variableIdx = popVar(&root);
		genConst(val);
		insertCommand("STORE ");
		insertCommandNum(variableIdx+1);
		insertCommand("\n");
	} else if (peekNumberVal == 0) {
		long long val = popVar(&root);
		variableIdx = popVar(&root);
		insertCommand("SUB 0\n");
		insertCommand("LOAD ");
		insertCommandNum(val);
		insertCommand("\nSTORE ");
		insertCommandNum(variableIdx+1);
		insertCommand("\nSUB 0\n");	
	} else {
		struct ideStack *iS = popIdeIde(&root);
		variableIdx = popVar(&root);
		long long wyr = iS->tabMemIdx - iS->tabIdx;
		genConst(wyr);
		insertCommand("ADD ");
		insertCommandNum(iS->memIdx);
		insertCommand("\nLOADI 0");
		insertCommand("\nSTORE ");
		insertCommandNum(variableIdx+1);
		insertCommand("\nSUB 0\n");
	}

	labelCounter++;
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	pushExpr(&root, labelCounter);
 
	insertCommand("SUB 0\n");
	insertCommand("LOAD ");
	insertCommandNum(variableIdx);
	insertCommand("\n");
	insertCommand("SUB ");
	insertCommandNum(variableIdx+1);
	insertCommand("\n");

	labelCounter++;
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);
}

void endFor(char *name) {

	// Zwiększ indeks o 1
	long long nameIdx = findVariable(name);
	insertCommand("SUB 0\n");
	insertCommand("INC\n");
	insertCommand("ADD ");
	insertCommandNum(nameIdx);
	insertCommand("\n");
	insertCommand("STORE ");
	insertCommandNum(nameIdx);
	insertCommand("\n");

	struct ideStack* iSMark = popIdeIde(&root);
	struct ideStack* iSJump = popIdeIde(&root);

	insertCommand("JUMP ");
	insertCommand("#");
	insertCommandNum(iSJump->label);
	insertCommand("\n");

	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
}

void endForDownTo(char* name) {
	// Zmniejsz indeks o 1
	long long nameIdx = findVariable(name);
	insertCommand("SUB 0\n");
	insertCommand("DEC\n");
	insertCommand("ADD ");
	insertCommandNum(nameIdx);
	insertCommand("\n");
	insertCommand("STORE ");
	insertCommandNum(nameIdx);
	insertCommand("\n");

	struct ideStack* iSMark = popIdeIde(&root);
	struct ideStack* iSJump = popIdeIde(&root);

	insertCommand("JUMP ");
	insertCommand("#");
	insertCommandNum(iSJump->label);
	insertCommand("\n");

	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
}

// obługa polecenia ASSIGN
void assignVariable() {
	int peekNumberVal = peekIsNumber(root);
	long long localValue;
	long long ideTabIdx;
	if (peekNumberVal == 1) {  		// Wartość przypisywana do zmiennej jest liczbą
		localValue = popNum(&root);
		int isVariable = peekIsNumber(root);
		if (isVariable == 0) {		// IDE = NUM
			ideTabIdx = popVar(&root);
			genConst(localValue);
			insertCommand("STORE ");
			insertCommandNum(ideTabIdx);
			insertCommand("\nSUB 0\n");
		} else {		// IDE(IDE) = NUM
			struct ideStack *iS = popIdeIde(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nSTORE 2");
			// insertCommandNum(memIdx);
			insertCommand("\nSUB 0\n"); 
			genConst(localValue);
			insertCommand("STOREI 2");
			// insertCommandNum(2);
			insertCommand("\nSUB 0\n");
		}
		
	} else if(peekNumberVal == 0) {
		localValue = popVar(&root);
		int isVariable = peekIsNumber(root);
		if (isVariable == 0) {		// SYTUACJA IDE = IDE;
			ideTabIdx = popVar(&root);
			insertCommand("SUB 0\n");
			insertCommand("LOAD ");
			insertCommandNum(localValue);
			insertCommand("\nSTORE ");
			insertCommandNum(ideTabIdx);
			insertCommand("\nSUB 0\n");		
		} else if (isVariable == 2) {  // Sytuacja IDE(IDE) = IDE
			// zapis adresu do bufora (memIdx)
			struct ideStack *iS = popIdeIde(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\nSUB 0\n");
			insertCommand("LOAD ");
			insertCommandNum(localValue);
			insertCommand("\nSTOREI ");	
			insertCommandNum(memIdx);
			insertCommand("\nSUB 0\n");
		}
	} else if (peekNumberVal == 2) {	// Wartość przypisywana jest w formacie IDE(IDE)
		struct ideStack *iS = popIdeIde(&root);
		int isVariable = peekIsNumber(root);
		if (isVariable == 0) { 	// IDE = IDE(IDE)
			ideTabIdx = popVar(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nLOADI 0\n");
			insertCommand("STORE ");
			insertCommandNum(ideTabIdx);
			insertCommand("\nSUB 0\n");
		} else if (isVariable == 2) {	// Sytuacja IDE(IDE) ASSIGN IDE(IDE)
			struct ideStack *identifier = popIdeIde(&root);
			long long wyr = identifier->tabMemIdx - identifier->tabIdx;
			insertCommand("SUB 0\n");
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(identifier->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\nSUB 0\n");
			
			wyr = iS->tabMemIdx - iS->tabIdx ;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nLOADI 0\n"); // Po tym jest wartość w p0
			insertCommand("STOREI ");
			insertCommandNum(memIdx);
			insertCommand("\nSUB 0\n");			
		} else {
			yyerror("Próba przypisania wartości do stałej");
		}
	} else {
		yyerror("Próba przypisania wartości z niezainicjowanej zmiennej");
	}
}

// obsługa polecenia READ
void readVariable()
{
	int peekNumberVal = peekIsNumber(root);
	if (peekNumberVal == 0) {
		long long ideTabIdx = popVar(&root);
		insertCommand("GET\n");
		insertCommand("STORE ");
		insertCommandNum(ideTabIdx);
		insertCommand("\n");
	} else if (peekNumberVal == 1) {
		yyerror("Próba przypisania wartości do stałej");	
	} else {
		struct ideStack *identifier = popIdeIde(&root);
		long long wyr = identifier->tabMemIdx - identifier->tabIdx;
		insertCommand("SUB 0\n");
		genConst(wyr);
		insertCommand("ADD ");
		insertCommandNum(identifier->memIdx);
		insertCommand("\nSTORE ");
		insertCommandNum(memIdx);
		insertCommand("\nSUB 0\n");
		insertCommand("GET\n");
		insertCommand("STOREI ");
		insertCommandNum(memIdx);
		insertCommand("\n");
	}
}

void writeVariable()
{
	int peekNumberVal = peekIsNumber(root);
	if (peekNumberVal == 1) {
		long long localValue = popNum(&root);
		genConst(localValue);
		insertCommand("PUT\nSUB 0\n");
	} else if (peekNumberVal == 0) {
		long long localValue = popVar(&root);
		insertCommand("LOAD ");
		insertCommandNum(localValue);
		insertCommand("\nPUT\nSUB 0\n");
	} else {
		struct ideStack *identifier = popIdeIde(&root);
		long long wyr = identifier->tabMemIdx - identifier->tabIdx;
		genConst(wyr);
		insertCommand("ADD ");
		insertCommandNum(identifier->memIdx);
		insertCommand("\nLOADI 0");
		insertCommand("\nPUT\nSUB 0\n");
	}
}

void comIf() {
	insertCommandNum(lines);
	insertCommand("\n");
}

void expMinus() {
	long long firstValue;
	long long secondValue;
	int peekNumberVal = peekIsNumber(root);
	if (peekNumberVal == 1) {
		firstValue = popNum(&root);
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 1) {
			// NUM - NUM
			secondValue = popNum(&root);
			genConst(firstValue-secondValue);
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else if (peekNumberVal == 0) {
			// Sytuacja IDE - NUM
			secondValue = popVar(&root);
			genConst(firstValue);
			insertCommand("STORE 2\n");
			insertCommand("LOAD ");
			insertCommandNum(secondValue);
			insertCommand("\n");
			insertCommand("SUB 2\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else if (peekNumberVal == 2) {
			// IDE(IDE) - Num
			struct ideStack *iS = popIdeIde(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			genConst(firstValue);
			insertCommand("STORE 2\n");
			insertCommand("LOADI ");
			insertCommandNum(memIdx);
			insertCommand("\nSUB 2");
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		}
	} else if (peekNumberVal == 0) {
		firstValue = popVar(&root);
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 0) { 
		// IDE - IDE
		secondValue = popVar(&root);
			insertCommand("SUB 0\n");
			insertCommand("ADD ");
			insertCommandNum(secondValue);
			insertCommand("\n");
			insertCommand("SUB ");
			insertCommandNum(firstValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else if (peekNumberVal == 1) { 
			//  NUM - IDE
			secondValue = popNum(&root);
			genConst(secondValue);
			insertCommand("SUB ");
			insertCommandNum(firstValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else {
			// IDE(IDE) -  IDE
			struct ideStack *iS = popIdeIde(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			insertCommand("LOADI ");
			insertCommandNum(memIdx);
			insertCommand("\nSUB ");
			insertCommandNum(firstValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		}
	} else {
		struct ideStack *iS = popIdeIde(&root);
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 1) {
			// NUM - IDE(IDE)
			secondValue = popNum(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nLOADI 0");
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			genConst(secondValue);
			insertCommand("SUB ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else if (peekNumberVal == 0) {
			// IDE - IDE(IDE)
			secondValue = popVar(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nLOADI 0");
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			insertCommand("\nLOAD ");
			insertCommandNum(secondValue);
			insertCommand("\nSUB ");
			insertCommandNum(memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;

		} else {
			// IDE(IDE) - IDE(IDE)
			struct ideStack *ide = popIdeIde(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nLOADI 0");
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			wyr = ide->tabMemIdx - ide->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(ide->memIdx);
			insertCommand("\nLOADI 0\n");

			insertCommand("SUB ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		}
	}
}

void expPlus() {
	long long firstValue;
	long long secondValue;
	int peekNumberVal = peekIsNumber(root);
	if (peekNumberVal == 1) {
		firstValue = popNum(&root);
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 1) {
			// NUM - NUM
			secondValue = popNum(&root);
			genConst(firstValue+secondValue);
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else if (peekNumberVal == 0) {
			// Sytuacja IDE + NUM
			secondValue = popVar(&root);
			genConst(firstValue);
			insertCommand("ADD ");
			insertCommandNum(secondValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else if (peekNumberVal == 2) {
			// IDE(IDE) - Num
			struct ideStack *iS = popIdeIde(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			genConst(firstValue);
			insertCommand("STORE 2\n");
			insertCommand("LOADI ");
			insertCommandNum(memIdx);
			insertCommand("\nADD 2");
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		}
	} else if (peekNumberVal == 0) {
		firstValue = popVar(&root);
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 0) { 
		// IDE - IDE
		secondValue = popVar(&root);
			insertCommand("SUB 0\n");
			insertCommand("ADD ");
			insertCommandNum(firstValue);
			insertCommand("\n");
			insertCommand("ADD ");
			insertCommandNum(secondValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else if (peekNumberVal == 1) { 
			//  NUM - IDE
			secondValue = popNum(&root);
			genConst(secondValue);
			insertCommand("\nADD ");
			insertCommandNum(firstValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else {
			// IDE(IDE) - IDE
			struct ideStack *iS = popIdeIde(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			insertCommand("LOADI ");
			insertCommandNum(memIdx);
			insertCommand("\nADD ");
			insertCommandNum(firstValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		}
	} else {
		struct ideStack *iS = popIdeIde(&root);
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 1) {
			// NUM + IDE(IDE)
			secondValue = popNum(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			genConst(secondValue);
			insertCommand("STORE 2\n");
			insertCommand("LOADI ");
			insertCommandNum(memIdx);
			insertCommand("\nADD 2");
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else if (peekNumberVal == 0) {
			// IDE + IDE(IDE)
			secondValue = popVar(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			insertCommand("LOADI ");
			insertCommandNum(memIdx);
			insertCommand("\nADD ");
			insertCommandNum(secondValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		} else {
			// IDE(IDE) + IDE(IDE)
			struct ideStack *ide = popIdeIde(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			wyr = ide->tabMemIdx - ide->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(ide->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx+1);
			insertCommand("\n");

			insertCommand("LOADI ");
			insertCommandNum(memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			insertCommand("LOADI ");
			insertCommandNum(memIdx+1);
			insertCommand("\nADD ");
			insertCommandNum(memIdx);

			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			pushVal(&root, 0, memIdx, 1);
			memIdx++;
		}
	}
}

void expDivNew() {
	// Zapisujemy pierwszą w komórce memIdx, drugą w komórce memIdx+1. Najpierw ściągamy drugą!
	int peekNumberVal;
	long long secondValue;
	long long wyr;
	long long endLabel;
	long long zerosJumpLabel;
	struct ideStack* ide;
	struct ideStack* iS;
	insertCommand("SUB 0\nDEC\nSTORE 8\nINC\nSTORE ");
	insertCommandNum(memIdx+3);
	insertCommand("\nSTORE 5\nINC\nSTORE 4\nINC\nSTORE ");
	insertCommandNum(memIdx+2);
	insertCommand("\n");
	for(int i=1; i>=0; i--) {
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 0) {
			secondValue = popVar(&root);
			insertCommand("LOAD ");
			insertCommandNum(secondValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx+i);
			insertCommand("\n");
		} else if (peekNumberVal == 1) {
			secondValue = popNum(&root);
			genConst(secondValue);
			insertCommand("STORE ");
			insertCommandNum(memIdx+i);
			insertCommand("\n");
		} else {
			ide = popIdeIde(&root);
			wyr = ide->tabMemIdx - ide->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(ide->memIdx);
			insertCommand("\nLOADI 0\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx+i);
			insertCommand("\n");
		}

		// Obsługa zer
		if(i==1) {
			labelCounter++; 
			zerosJumpLabel = labelCounter;
		}
		insertCommand("JZERO ");
		insertCommand("#");
		insertCommandNum(zerosJumpLabel);
		insertCommand("\n");

		// tu jest value w p0 -> sprawdź ujemność i dodaj do mem+2
		labelCounter++;
		insertCommand("JPOS ");
		insertCommand("#");
		insertCommandNum(labelCounter);
		insertCommand("\n");
		pushExpr(&root, labelCounter);
		// Dodatnia to skacz, ujemna to dodaj ją dwa razy
		insertCommand("STORE 2\n");
		insertCommand("SUB 2\n");
		insertCommand("SUB 2\n");
		insertCommand("STORE ");
		insertCommandNum(memIdx+i);
		insertCommand("\n");
		insertCommand("SUB 0\nDEC\nDEC\nADD ");
		insertCommandNum(memIdx+2);
		insertCommand("\nSTORE ");
		insertCommandNum(memIdx+2);
		insertCommand("\n");
		// Tu skacz
		iS = popIdeIde(&root);
		insertCommand("#");
		insertCommandNum(iS->label);
		insertCommand(" ");
		insertCommandNum(lines);
		insertCommand(" ");
	}


	// memIdx i w memIdx+1 mamy wartości bezwzględne, memIdx+2 przechowuje informacje o znakach
	insertCommand("SUB 0\n");
	labelCounter++;
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	pushExpr(&root, labelCounter);

	// sprawdź czy a-b >= 0
	insertCommand("LOAD ");
	insertCommandNum(memIdx);
	insertCommand("\n");
	insertCommand("SUB ");
	insertCommandNum(memIdx+1);
	insertCommand("\n");

	labelCounter++;
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	// zapisz b do bufora (5)
	insertCommand("LOAD ");
	insertCommandNum(memIdx+1);
	insertCommand("\nSTORE 5\nSUB 0\nINC\nSTORE 7\n");

	labelCounter++;
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	pushExpr(&root, labelCounter);

	// start pętli
	insertCommand("LOAD 7\nSHIFT 4\nSTORE 7\nLOAD 5\nSHIFT 4\nSTORE 5\n"); // bufor *= 2
	insertCommand("SUB ");
	insertCommandNum(memIdx);
	insertCommand("\n");

	struct ideStack* iSMark = popIdeIde(&root);
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand("\n");


	//bufor:2
	insertCommand("LOAD 5\nSHIFT 8\nSTORE 5\n"); 

	// a = a - bufor
	insertCommand("LOAD ");
	insertCommandNum(memIdx);
	insertCommand("\nSUB 5\nSTORE ");
	insertCommandNum(memIdx);

	// wynik += bufor;
	insertCommand("\nLOAD 7\nSHIFT 8\n");
	insertCommand("ADD ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	insertCommand("STORE ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");


	iSMark = popIdeIde(&root);
	struct ideStack* iSJump = popIdeIde(&root);
	insertCommand("JUMP ");
	insertCommand("#");
	insertCommandNum(iSJump->label);
	insertCommand("\n");

	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");

	insertCommand("SUB 0\n");
	//koniec zer -> tu skacz
	
	insertCommand("#");
	insertCommandNum(zerosJumpLabel);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");

	insertCommand("LOAD ");
	insertCommandNum(memIdx+2);
	insertCommand("\n");
	labelCounter++;

	// Jeśli te same znaki to pomiń
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	insertCommand("JPOS ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	insertCommand("LOAD ");
	insertCommandNum(memIdx);
	insertCommand("\n");

	// Jeśli 0 to przeskocz;
	labelCounter++; 
	insertCommand("JZERO ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	pushExpr(&root, labelCounter);
	insertCommand("\n");
	insertCommand("LOAD ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	insertCommand("INC\n");
	insertCommand("STORE ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	iSMark = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	insertCommand("SUB 0\n");
	insertCommand("SUB ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	insertCommand("STORE ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");

	iSMark = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	insertCommand("LOAD ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	insertCommand("STORE ");
	insertCommandNum(memIdx);
	insertCommand("\n");
	pushVal(&root, 0, memIdx, 1);
	memIdx++;
}

void expDiv() {
	// Zapisujemy pierwszą w komórce memIdx, drugą w komórce memIdx+1. Najpierw ściągamy drugą!
	int peekNumberVal;
	long long secondValue;
	long long wyr;
	long long endLabel;
	long long zerosJumpLabel;
	struct ideStack* ide;
	struct ideStack* iS;
	insertCommand("SUB 0\nSTORE ");
	insertCommandNum(memIdx+3);
	insertCommand("\nINC\nINC\nSTORE ");
	insertCommandNum(memIdx+2);
	insertCommand("\n");
	for(int i=1; i>=0; i--) {
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 0) {
			secondValue = popVar(&root);
			insertCommand("LOAD ");
			insertCommandNum(secondValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx+i);
			insertCommand("\n");
		} else if (peekNumberVal == 1) {
			secondValue = popNum(&root);
			genConst(secondValue);
			insertCommand("STORE ");
			insertCommandNum(memIdx+i);
			insertCommand("\n");
		} else {
			ide = popIdeIde(&root);
			wyr = ide->tabMemIdx - ide->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(ide->memIdx);
			insertCommand("\nLOADI 0\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx+i);
			insertCommand("\n");
		}

		// Obsługa zer
		if(i==1) {
			labelCounter++; 
			zerosJumpLabel = labelCounter;
		}
		insertCommand("JZERO ");
		insertCommand("#");
		insertCommandNum(zerosJumpLabel);
		insertCommand("\n");

		// tu jest value w p0 -> sprawdź ujemność i dodaj do mem+2
		labelCounter++;
		insertCommand("JPOS ");
		insertCommand("#");
		insertCommandNum(labelCounter);
		insertCommand("\n");
		pushExpr(&root, labelCounter);
		// Dodatnia to skacz, ujemna to dodaj ją dwa razy
		insertCommand("STORE 2\n");
		insertCommand("SUB 2\n");
		insertCommand("SUB 2\n");
		insertCommand("STORE ");
		insertCommandNum(memIdx+i);
		insertCommand("\n");
		insertCommand("SUB 0\nDEC\nDEC\nADD ");
		insertCommandNum(memIdx+2);
		insertCommand("\nSTORE ");
		insertCommandNum(memIdx+2);
		insertCommand("\n");
		// Tu skacz
		iS = popIdeIde(&root);
		insertCommand("#");
		insertCommandNum(iS->label);
		insertCommand(" ");
		insertCommandNum(lines);
		insertCommand(" ");
	}


	// memIdx i w memIdx+1 mamy wartości bezwzględne, memIdx+2 przechowuje informacje o znakach
	insertCommand("SUB 0\n");
	labelCounter++;
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	pushExpr(&root, labelCounter);

	// wykonaj (a - b) i zapisz jako nowe a
	insertCommand("LOAD ");
	insertCommandNum(memIdx);
	insertCommand("\n");
	insertCommand("SUB ");
	insertCommandNum(memIdx+1);
	insertCommand("\n");
	insertCommand("STORE ");
	insertCommandNum(memIdx);
	insertCommand("\n");


	labelCounter++;
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	// wynik += 1;
	insertCommand("SUB 0\n");
	insertCommand("INC\n");
	insertCommand("ADD ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	insertCommand("STORE ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");


	struct ideStack* iSMark = popIdeIde(&root);
	struct ideStack* iSJump = popIdeIde(&root);
	insertCommand("JUMP ");
	insertCommand("#");
	insertCommandNum(iSJump->label);
	insertCommand("\n");

	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");

	insertCommand("SUB 0\n");
	//koniec zer -> tu skacz
	
	insertCommand("#");
	insertCommandNum(zerosJumpLabel);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");

	insertCommand("LOAD ");
	insertCommandNum(memIdx+2);
	insertCommand("\n");
	labelCounter++;

	// Jeśli te same znaki to pomiń
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	insertCommand("JPOS ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	insertCommand("LOAD ");
	insertCommandNum(memIdx);
	insertCommand("\n");

	// Jeśli 0 to przeskocz;
	labelCounter++; 
	insertCommand("JZERO ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	pushExpr(&root, labelCounter);
	insertCommand("\n");
	insertCommand("LOAD ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	insertCommand("INC\n");
	insertCommand("STORE ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	iSMark = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	insertCommand("SUB 0\n");
	insertCommand("SUB ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	insertCommand("STORE ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");

	iSMark = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	insertCommand("LOAD ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	insertCommand("STORE ");
	insertCommandNum(memIdx);
	insertCommand("\n");
	pushVal(&root, 0, memIdx, 1);
	memIdx++;
}

void expMod() {
	// Zapisujemy pierwszą w komórce memIdx, drugą w komórce memIdx+1. Najpierw ściągamy drugą!
	int peekNumberVal;
	long long secondValue;
	long long wyr;
	long long endLabel;
	long long zerosJumpLabel;
	struct ideStack* ide;
	struct ideStack* iS;
	
	insertCommand("SUB 0\nDEC\nSTORE 8\nINC\n");
	insertCommand("INC\nSTORE 4\nINC\nSTORE ");
	insertCommandNum(memIdx+2);
	insertCommand("\n");
	insertCommand("SUB 0\n");
	insertCommand("INC\nSTORE ");
	insertCommandNum(memIdx+3);
	insertCommand("\n");
	for(int i=1; i>=0; i--) {
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 0) {
			secondValue = popVar(&root);
			insertCommand("LOAD ");
			insertCommandNum(secondValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx+i);
			insertCommand("\n");
		} else if (peekNumberVal == 1) {
			secondValue = popNum(&root);
			genConst(secondValue);
			insertCommand("STORE ");
			insertCommandNum(memIdx+i);
			insertCommand("\n");
		} else {
			ide = popIdeIde(&root);
			wyr = ide->tabMemIdx - ide->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(ide->memIdx);
			insertCommand("\nLOADI 0\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx+i);
			insertCommand("\n");
		}

		// Obsługa zer
		if(i==1) {
			labelCounter++; 
			zerosJumpLabel = labelCounter;
		}
		insertCommand("JZERO ");
		insertCommand("#");
		insertCommandNum(zerosJumpLabel);
		insertCommand("\n");

		// Sprawdź ujemność drugiej i wpisz do mem+3 
		if(i==1) {
			labelCounter++;
			insertCommand("JPOS ");
			insertCommand("#");
			insertCommandNum(labelCounter);
			insertCommand("\n");
			pushExpr(&root, labelCounter);

			insertCommand("SUB 0\nDEC\nSTORE ");
			insertCommandNum(memIdx+3);
			insertCommand("\n");

			iS = popIdeIde(&root);
			insertCommand("#");
			insertCommandNum(iS->label);
			insertCommand(" ");
			insertCommandNum(lines);
			insertCommand(" ");
			insertCommand("LOAD ");
			insertCommandNum(memIdx+1);
			insertCommand("\n");
		}
		// tu jest value w p0 -> sprawdź ujemność i dodaj do mem+2
		labelCounter++;
		insertCommand("JPOS ");
		insertCommand("#");
		insertCommandNum(labelCounter);
		insertCommand("\n");
		pushExpr(&root, labelCounter);
		// Dodatnia to skacz, ujemna to dodaj ją dwa razy
		insertCommand("STORE 2\n");
		insertCommand("SUB 2\n");
		insertCommand("SUB 2\n");
		insertCommand("STORE ");
		insertCommandNum(memIdx+i);
		insertCommand("\n");
		insertCommand("SUB 0\nDEC\nDEC\nADD ");
		insertCommandNum(memIdx+2);
		insertCommand("\nSTORE ");
		insertCommandNum(memIdx+2);
		insertCommand("\n");
		// Tu skacz
		iS = popIdeIde(&root);
		insertCommand("#");
		insertCommandNum(iS->label);
		insertCommand(" ");
		insertCommandNum(lines);
		insertCommand(" ");
	}

	// memIdx i w memIdx+1 mamy wartości bezwzględne, memIdx+2 przechowuje informacje o znakach

	insertCommand("SUB 0\n");
	labelCounter++;
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	pushExpr(&root, labelCounter);

	// sprawdź czy a-b >= 0
	insertCommand("LOAD ");
	insertCommandNum(memIdx);
	insertCommand("\n");
	insertCommand("SUB ");
	insertCommandNum(memIdx+1);
	insertCommand("\n");

	labelCounter++;
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	// zapisz b do bufora (5)
	insertCommand("LOAD ");
	insertCommandNum(memIdx+1);
	insertCommand("\nSTORE 5\nSUB 0\n");

	labelCounter++;
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	pushExpr(&root, labelCounter);

	// start pętli
	insertCommand("LOAD 5\nSHIFT 4\nSTORE 5\n"); // bufor *= 2
	insertCommand("SUB ");
	insertCommandNum(memIdx);
	insertCommand("\n");

	struct ideStack* iSMark = popIdeIde(&root);
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand("\n");


	//bufor:2
	insertCommand("LOAD 5\nSHIFT 8\nSTORE 5\n"); 

	// a = a - bufor
	insertCommand("LOAD ");
	insertCommandNum(memIdx);
	insertCommand("\nSUB 5\nSTORE ");
	insertCommandNum(memIdx);
	insertCommand("\n");



	iSMark = popIdeIde(&root);
	struct ideStack* iSJump = popIdeIde(&root);
	insertCommand("JUMP ");
	insertCommand("#");
	insertCommandNum(iSJump->label);
	insertCommand("\n");

	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");

	// TU jest zmienna teraz
	insertCommand("LOAD ");
	insertCommandNum(memIdx);
	insertCommand("\n");
	insertCommand("STORE ");
	insertCommandNum(memIdx);
	insertCommand("\n");

	


	// Jeśli 0 to przeskocz;
	labelCounter++; 
	insertCommand("JZERO ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	pushExpr(&root, labelCounter);
	insertCommand("\n");

		// insertCommand("LOAD ");
		// insertCommandNum(memIdx);
		// insertCommand("\n");
		// insertCommand("ADD ");
		// insertCommandNum(memIdx+1);
		// insertCommand("\n");
		// insertCommand("STORE ");
		// insertCommandNum(memIdx);
		// insertCommand("\n");

		insertCommand("LOAD ");
		insertCommandNum(memIdx+2);
		insertCommand("\n");
		labelCounter++;
		// Jeśli te same znaki to pomiń
		insertCommand("JNEG ");
		insertCommand("#");
		insertCommandNum(labelCounter);
		insertCommand("\n");
		insertCommand("JPOS ");
		insertCommand("#");
		insertCommandNum(labelCounter);
		insertCommand("\n");
		pushExpr(&root, labelCounter);

		// różne znaki -> reszta = reszta - druga liczba
		insertCommand("LOAD ");
		insertCommandNum(memIdx);
		insertCommand("\n");
		insertCommand("SUB ");
		insertCommandNum(memIdx+1);
		insertCommand("\n");
		insertCommand("STORE ");
		insertCommandNum(memIdx);
		insertCommand("\n");
		insertCommand("SUB ");
		insertCommandNum(memIdx);
		insertCommand("\n");
		insertCommand("SUB ");
		insertCommandNum(memIdx);
		insertCommand("\n");
		insertCommand("STORE ");
		insertCommandNum(memIdx);
		insertCommand("\n");

		iSMark = popIdeIde(&root);
		insertCommand("#");
		insertCommandNum(iSMark->label);
		insertCommand(" ");
		insertCommandNum(lines);
		insertCommand(" ");

		//druga ujemna -> pomnóż przez -1
		insertCommand("LOAD ");
		insertCommandNum(memIdx+3);
		insertCommand("\n");
		labelCounter++;
		insertCommand("JPOS ");
		insertCommand("#");
		insertCommandNum(labelCounter);
		insertCommand("\n");
		pushExpr(&root, labelCounter);

		insertCommand("LOAD ");
		insertCommandNum(memIdx);
		insertCommand("\n");
		insertCommand("SUB ");
		insertCommandNum(memIdx);
		insertCommand("\n");
		insertCommand("SUB ");
		insertCommandNum(memIdx);
		insertCommand("\n");
		insertCommand("STORE ");
		insertCommandNum(memIdx);
		insertCommand("\n");

		iSMark = popIdeIde(&root);
		insertCommand("#");
		insertCommandNum(iSMark->label);
		insertCommand(" ");
		insertCommandNum(lines);
		insertCommand(" ");


	iSMark = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");

	//przeskocz poniżej

	labelCounter++; 
	insertCommand("JUMP ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	//koniec zer -> tu skacz
	insertCommand("#");
	insertCommandNum(zerosJumpLabel);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	insertCommand("SUB 0\nSTORE ");
	insertCommandNum(memIdx);
	insertCommand("\n");

	// tu skacz
	iSMark = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	insertCommand("SUB 0\n");
	pushVal(&root, 0, memIdx, 1);
	memIdx++;
}

void expTimes() {
	int peekNumber = peekIsNumber(root);
	long long value;
	long long wyr;
	labelCounter++;
	long long zerosLabel = labelCounter;
	struct ideStack* ide;
	struct ideStack* iSMark;
	struct ideStack* iSJump;

	// wrzuć do 4 wartość 1 i do 5 wartość -1
	insertCommand("SUB 0\nINC\nSTORE 4\nDEC\nDEC\nSTORE 5\n");

	// wrzuć do 8 wartość 0 (wynik mnożenia) i wrzuć do 9 wartość 1 (informacja o znakach)
	insertCommand("SUB 0\nSTORE 8\nINC\nSTORE 9\n");


	if (peekNumber == 0) {
		// Zmienna
		value = popVar(&root);
		insertCommand("LOAD ");
		insertCommandNum(value);
		insertCommand("\n");
		insertCommand("STORE 7\n");
	} else if (peekNumber == 1) {
		value = popNum(&root);
		genConst(value);
		insertCommand("STORE 7\n");
	} else {
		ide = popIdeIde(&root);
		wyr = ide->tabMemIdx - ide->tabIdx;
		genConst(wyr);
		insertCommand("ADD ");
		insertCommandNum(ide->memIdx);
		insertCommand("\nLOADI 0\n");
		insertCommand("STORE 7\n");
	}

	insertCommand("JZERO ");
	insertCommand("#");
	insertCommandNum(zerosLabel);
	insertCommand("\n");

	peekNumber = peekIsNumber(root);
	if (peekNumber == 0) {
		// Zmienna
		value = popVar(&root);
		insertCommand("LOAD ");
		insertCommandNum(value);
		insertCommand("\n");
		insertCommand("STORE 6\n");
	} else if (peekNumber == 1) {
		value = popNum(&root);
		genConst(value);
		insertCommand("STORE 6\n");
	} else {
		ide = popIdeIde(&root);
		wyr = ide->tabMemIdx - ide->tabIdx;
		genConst(wyr);
		insertCommand("ADD ");
		insertCommandNum(ide->memIdx);
		insertCommand("\nLOADI 0\n");
		insertCommand("STORE 6\n");
	}

	insertCommand("JZERO ");
	insertCommand("#");
	insertCommandNum(zerosLabel);
	insertCommand("\n");


	// TUTAJ sprawdzamy znaki -> robimy z liczby bezwzględną i p(9)--
	insertCommand("LOAD 6\n");

	// jeśli > 0 to nie rób nic -> skacz niżej
	labelCounter++;
	insertCommand("JPOS ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	insertCommand("SUB 6\nSUB 6\nSTORE 6\n");
	insertCommand("LOAD 9\nDEC\nSTORE 9\n");

	// tu skacz
	iSMark = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");

	insertCommand("LOAD 7\n");

	labelCounter++;
	insertCommand("JPOS ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	insertCommand("SUB 7\nSUB 7\nSTORE 7\n");
	insertCommand("LOAD 9\nDEC\nSTORE 9\n");

	iSMark = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	insertCommand("INC\n");

	// TERAZ p(4) = 1 p(5) = -1 p(6) = X p(7) = Y p(8) = 0 
	
	// Jeśli p(7) == 1 lub -1 to przeskocz na koniec
	labelCounter++;
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	pushExpr(&root, labelCounter);
	
	// warunek p(7) == 1:
	insertCommand("LOAD 7\nDEC\n");
	labelCounter++;
	insertCommand("JZERO ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	insertCommand("LOAD 7\nSHIFT 5\nSHIFT 4\nSUB 7\n");
	// TERAZ w p(0) mamy 0 (Y parzysta) lub -1 (Y nieparzysta)

	// Jeśli parzysta to przeskocz dodawanie licznika
	labelCounter++;
	insertCommand("JZERO ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	// Dodaj do wyniku
	insertCommand("LOAD 6\nADD 8\nSTORE 8\n");

	iSMark = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");


	// Pomnóż p(6) * 2 i p(7) * 1/2
	insertCommand("LOAD 6\nSHIFT 4\nSTORE 6\n");
	insertCommand("LOAD 7\nSHIFT 5\nSTORE 7\n");

	iSMark = popIdeIde(&root);
	iSJump = popIdeIde(&root);

	insertCommand("LOAD 8\n");
	// SKACZ do góry
	insertCommand("JUMP ");
	insertCommand("#");
	insertCommandNum(iSJump->label);
	insertCommand("\n");

	// tu skaczemy z Y=|1| -> koniec :)
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");

	// dodaj ostatnią wartość do licznika
	insertCommand("LOAD 8\nADD 6\nSTORE 8\n");

	// uwzględnij znaki: jesli p(9) == 0 to zrób ujemną
	insertCommand("LOAD 9\n");
	labelCounter++;
	insertCommand("JPOS ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);

	insertCommand("LOAD 8\n");
	insertCommand("SUB 8\nSUB 8\n");
	insertCommand("STORE 8\n");

	iSMark = popIdeIde(&root);
	insertCommand("#");
	insertCommandNum(iSMark->label);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");

	insertCommand("INC\n");


	insertCommand("#");
	insertCommandNum(zerosLabel);
	insertCommand(" ");
	insertCommandNum(lines);
	insertCommand(" ");
	insertCommand("SUB 0\n");
	pushVal(&root, 0, 8, 1);
}

void getCondValueInFirstMemo() { // A COND B -> VALUE: A - B
	int peekNumberVal = peekIsNumber(root);
	if (peekNumberVal == 0) {
		// ? IDE
		long long firstValue = popVar(&root);
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 0) {
			// IDE - IDE
			long long secondValue = popVar(&root);
			insertCommand("SUB 0\n");
			insertCommand("ADD ");
			insertCommandNum(secondValue);
			insertCommand("\n");
			insertCommand("SUB ");
			insertCommandNum(firstValue);
			insertCommand("\n");
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
		} else if (peekNumberVal == 1) {
			// NUM - IDE
			long long secondValue = popNum(&root);
			genConst(secondValue);
			insertCommand("SUB ");
			insertCommandNum(firstValue);
			insertCommand("\n");
		} else {
			// IDE(IDE) - IDE
			struct ideStack *iS = popIdeIde(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nLOADI 0\n");
			insertCommand("SUB ");
			insertCommandNum(firstValue);
			insertCommand("\n");
		}
	} else if (peekNumberVal == 1) {
		// ? NUM
		long long firstValue = popNum(&root);
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 0) {
			// IDE NUM
			long long secondValue = popVar(&root);
			genConst(firstValue);
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			insertCommand("LOAD ");
			insertCommandNum(secondValue);
			insertCommand("\n");

			insertCommand("SUB ");
			insertCommandNum(memIdx);
			insertCommand("\n");
		} else if (peekNumberVal == 1) {
			// NUM NUM 
			long long secondValue = popNum(&root);
			genConst(firstValue);
			insertCommand("STORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");
			genConst(secondValue);
			insertCommand("SUB ");
			insertCommandNum(memIdx);
			insertCommand("\n");
		} else {
			// IDE(IDE) - NUM
			struct ideStack *iS = popIdeIde(&root);

			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			genConst(firstValue);
			insertCommand("STORE 2\n");
			insertCommand("LOADI ");
			insertCommandNum(memIdx);
			insertCommand("\nSUB 2\n");
		}
	} else {
		struct ideStack *iS = popIdeIde(&root);
		peekNumberVal = peekIsNumber(root);
		if (peekNumberVal == 1) {
			// NUM - IDE(IDE)
			long long secondValue = popNum(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nLOADI 0");
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			genConst(secondValue);
			insertCommand("SUB ");
			insertCommandNum(memIdx);
			insertCommand("\n");
		} else if (peekNumberVal == 0) {
			// IDE - IDE(IDE)
			long long secondValue = popVar(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nLOADI 0");
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			insertCommand("\nLOAD ");
			insertCommandNum(secondValue);
			insertCommand("\nSUB ");
			insertCommandNum(memIdx);
		} else {
			// IDE(IDE) - IDE(IDE)
			struct ideStack *ide = popIdeIde(&root);
			long long wyr = iS->tabMemIdx - iS->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(iS->memIdx);
			insertCommand("\nLOADI 0");
			insertCommand("\nSTORE ");
			insertCommandNum(memIdx);
			insertCommand("\n");

			wyr = ide->tabMemIdx - ide->tabIdx;
			genConst(wyr);
			insertCommand("ADD ");
			insertCommandNum(ide->memIdx);
			insertCommand("\nLOADI 0\n");

			insertCommand("SUB ");
			insertCommandNum(memIdx);
			insertCommand("\n");
		}
	}

}

void condEq() {
	// TU już mamy wartość w p0	
	labelCounter++;
	getCondValueInFirstMemo();
	insertCommand("JPOS ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);
 }

void condNeq() {
	labelCounter++;
	getCondValueInFirstMemo();
	insertCommand("JZERO ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);
}

void condLE() {
	labelCounter++;
	getCondValueInFirstMemo();
	insertCommand("JPOS ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	insertCommand("JZERO ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);
}

void condGE() {
labelCounter++;
	getCondValueInFirstMemo();
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	insertCommand("JZERO ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);
}

void condLEQ() {
	labelCounter++;
	getCondValueInFirstMemo();
	insertCommand("JPOS ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);
}

void condGEQ() {
	labelCounter++;
	getCondValueInFirstMemo();
	insertCommand("JNEG ");
	insertCommand("#");
	insertCommandNum(labelCounter);
	insertCommand("\n");
	pushExpr(&root, labelCounter);
}

// Obsługa NUM
void numIdent(char *a)
{
	long long num = atoll(a);
	push(&root, 1, num);
}


// Obsługa IDE
void ident(char *a) {
	long long localIndex;
	localIndex = findVariable(a);
	if (localIndex == -1) {
		yyerror("Niezadeklarowania zmienna!");
	} else {
		push(&root, 0, localIndex);
	}
}

// Obsługa IDE(NUM)
void identNumTab(char *a, char* b) {
	long long num = atoll(b);
	// long long localIndex = findTabVariable(a, num); TODO: OLD
	long long localIndex = findArrayIndex(a, num);
	//printf("old: %lld, new: %lld \n", localIndex, localIndex2); TODO: OLD
	if (localIndex == -1) {
		yyerror("Tablica o tej nazwie bądź indeksie nie istnieje!");
	} else {
		push(&root, 0, localIndex);
	}
}

// Obsługa IDE(IDE)
void identIdentTab(char *a, char *b) {
	long long localIndex;
	localIndex = findVariable(b);
	if (localIndex == -1) {
		yyerror("Niezadeklarowania zmienna!");
	} else {
		struct symTable *st = findVariableByIndex(localIndex);
		if(st == NULL) {
			yyerror("Zmienna będąca indeksem tabeli jest niezadeklarowana!");
		} else {
			pushIdeIde(&root, 2, localIndex, a);
		}
	}
}


#line 2598 "bison.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "bison.tab.h".  */
#ifndef YY_YY_BISON_TAB_H_INCLUDED
# define YY_YY_BISON_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    DECLARE = 258,
    BGN = 259,
    END = 260,
    ASSIGN = 261,
    IF = 262,
    THEN = 263,
    ELSE = 264,
    ENDIF = 265,
    WHILE = 266,
    DO = 267,
    ENDWHILE = 268,
    ENDDO = 269,
    FOR = 270,
    FROM = 271,
    TO = 272,
    DOWNTO = 273,
    ENDFOR = 274,
    READ = 275,
    WRITE = 276,
    PLUS = 277,
    MINUS = 278,
    TIMES = 279,
    DIV = 280,
    MOD = 281,
    EQ = 282,
    NEQ = 283,
    LE = 284,
    GE = 285,
    LEQ = 286,
    GEQ = 287,
    SEM = 288,
    COM = 289,
    LB = 290,
    RB = 291,
    IDE = 292,
    NUM = 293,
    COL = 294
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 2532 "bison.y" /* yacc.c:355  */

    char *str;
    long long num;

#line 2683 "bison.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_BISON_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 2700 "bison.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  18
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   237

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  17
/* YYNRULES -- Number of rules.  */
#define YYNRULES  43
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  108

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   294

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,  2546,  2546,  2547,  2551,  2552,  2553,  2554,  2558,  2559,
    2563,  2564,  2565,  2566,  2567,  2568,  2569,  2570,  2571,  2575,
    2578,  2581,  2584,  2587,  2590,  2593,  2596,  2599,  2600,  2601,
    2602,  2603,  2604,  2608,  2609,  2610,  2611,  2612,  2613,  2617,
    2618,  2622,  2623,  2624
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DECLARE", "BGN", "END", "ASSIGN", "IF",
  "THEN", "ELSE", "ENDIF", "WHILE", "DO", "ENDWHILE", "ENDDO", "FOR",
  "FROM", "TO", "DOWNTO", "ENDFOR", "READ", "WRITE", "PLUS", "MINUS",
  "TIMES", "DIV", "MOD", "EQ", "NEQ", "LE", "GE", "LEQ", "GEQ", "SEM",
  "COM", "LB", "RB", "IDE", "NUM", "COL", "$accept", "program",
  "declarations", "commands", "command", "_else", "_while", "_do",
  "_forFrom", "_forTo", "_forDownTo", "_forDo", "_forDoDownTo",
  "expression", "condition", "value", "identifier", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294
};
# endif

#define YYPACT_NINF -32

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-32)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       1,   -25,   112,    16,    -5,    -1,   -31,   -32,   -32,   -13,
       0,   -31,    18,    39,   -32,   -31,   112,    26,   -32,    14,
     112,    21,   -32,    47,    40,   -32,    45,    29,    30,   -28,
     -32,   -32,    52,   200,   -31,    34,   110,    44,   112,   -31,
     -31,   -31,   -31,   -31,   -31,   -32,   -31,   -32,   -32,    38,
      46,   112,   -31,    32,    17,    37,   -32,    42,   148,   -32,
     -32,   -32,   -32,   -32,   -32,     9,   -32,   -32,     8,    71,
     -32,   -31,   -31,   -31,   -31,   -31,    48,    49,   -32,   -32,
     112,   -32,   -32,   -31,   -31,   -32,   -32,   -32,   -32,   -32,
     -32,   -32,   -32,    51,   155,    74,    75,    54,   -32,   -32,
     112,   -32,   112,   -32,   167,   182,   -32,   -32
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     6,     0,     0,    20,    21,     0,
       0,     0,    41,     0,     9,     0,     0,     0,     1,     0,
       0,     0,    39,     0,     0,    40,     0,     0,     0,     0,
       3,     8,     0,     0,     0,     0,     0,     4,     0,     0,
       0,     0,     0,     0,     0,    22,     0,    17,    18,     0,
       0,     0,     0,     0,    27,     0,     2,     0,     0,    33,
      34,    35,    36,    37,    38,     0,    42,    43,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,    19,    12,
       0,    23,    24,     0,     0,    13,    14,    28,    29,    30,
      31,    32,     7,     0,     0,     0,     0,     0,    11,    25,
       0,    26,     0,     5,     0,     0,    15,    16
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -32,   -32,   -32,    -3,   -11,   -32,   -32,   -32,   -32,   -32,
     -32,   -32,   -32,   -32,   -14,    70,    -2
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     5,    13,    14,    80,    15,    16,    46,    83,
      84,   100,   102,    53,    23,    24,    25
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      17,    32,    31,    20,     1,     2,    12,    22,    27,    49,
      50,    17,     4,    33,    17,     6,    18,    36,    17,     7,
       8,    85,    31,     9,    26,    31,    81,    82,    10,    11,
      19,    17,    34,    21,    17,    58,    17,    12,    69,    71,
      72,    73,    74,    75,    30,    12,     6,    31,    68,    17,
       7,     8,    35,    29,     9,    38,    17,    31,    37,    10,
      11,    45,    47,    48,    51,    70,    17,    39,    40,    41,
      42,    43,    44,    55,    66,    76,    12,    94,    17,    57,
      77,    28,    67,    31,    92,    86,    99,   101,    93,    97,
     103,     0,    17,    31,    31,     0,     0,   104,    17,   105,
      17,     0,    17,    17,    54,     0,     0,     0,     0,    59,
      60,    61,    62,    63,    64,    56,    65,     6,     0,     6,
       0,     7,     8,     7,     8,     9,     0,     9,     0,     0,
      10,    11,    10,    11,     0,     0,     0,     0,     0,     0,
       0,    87,    88,    89,    90,    91,     0,    12,     0,    12,
       0,     0,     0,    95,    96,     6,     0,    78,    79,     7,
       8,     0,     6,     9,     0,    98,     7,     8,    10,    11,
       9,     0,     0,     0,     6,    10,    11,     0,     7,     8,
       0,     0,     9,     0,     0,    12,   106,    10,    11,     6,
       0,     0,    12,     7,     8,     0,     0,     9,     0,     0,
       0,   107,    10,    11,    12,     0,     0,     6,     0,     0,
       0,    52,     8,     0,     0,     9,     0,     0,     0,    12,
      10,    11,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    12
};

static const yytype_int8 yycheck[] =
{
       2,    15,    13,     4,     3,     4,    37,    38,    10,    37,
      38,    13,    37,    16,    16,     7,     0,    20,    20,    11,
      12,    13,    33,    15,    37,    36,    17,    18,    20,    21,
      35,    33,     6,    34,    36,    38,    38,    37,    52,    22,
      23,    24,    25,    26,     5,    37,     7,    58,    51,    51,
      11,    12,    38,    35,    15,     8,    58,    68,    37,    20,
      21,    16,    33,    33,    12,    33,    68,    27,    28,    29,
      30,    31,    32,    39,    36,    38,    37,    80,    80,    35,
      38,    11,    36,    94,    36,    14,    12,    12,    39,    38,
      36,    -1,    94,   104,   105,    -1,    -1,   100,   100,   102,
     102,    -1,   104,   105,    34,    -1,    -1,    -1,    -1,    39,
      40,    41,    42,    43,    44,     5,    46,     7,    -1,     7,
      -1,    11,    12,    11,    12,    15,    -1,    15,    -1,    -1,
      20,    21,    20,    21,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    71,    72,    73,    74,    75,    -1,    37,    -1,    37,
      -1,    -1,    -1,    83,    84,     7,    -1,     9,    10,    11,
      12,    -1,     7,    15,    -1,    10,    11,    12,    20,    21,
      15,    -1,    -1,    -1,     7,    20,    21,    -1,    11,    12,
      -1,    -1,    15,    -1,    -1,    37,    19,    20,    21,     7,
      -1,    -1,    37,    11,    12,    -1,    -1,    15,    -1,    -1,
      -1,    19,    20,    21,    37,    -1,    -1,     7,    -1,    -1,
      -1,    11,    12,    -1,    -1,    15,    -1,    -1,    -1,    37,
      20,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    37
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,    41,    37,    42,     7,    11,    12,    15,
      20,    21,    37,    43,    44,    46,    47,    56,     0,    35,
       4,    34,    38,    54,    55,    56,    37,    56,    55,    35,
       5,    44,    54,    43,     6,    38,    43,    37,     8,    27,
      28,    29,    30,    31,    32,    16,    48,    33,    33,    37,
      38,    12,    11,    53,    55,    39,     5,    35,    43,    55,
      55,    55,    55,    55,    55,    55,    36,    36,    43,    54,
      33,    22,    23,    24,    25,    26,    38,    38,     9,    10,
      45,    17,    18,    49,    50,    13,    14,    55,    55,    55,
      55,    55,    36,    39,    43,    55,    55,    38,    10,    12,
      51,    12,    52,    36,    43,    43,    19,    19
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    40,    41,    41,    42,    42,    42,    42,    43,    43,
      44,    44,    44,    44,    44,    44,    44,    44,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    53,    53,
      53,    53,    53,    54,    54,    54,    54,    54,    54,    55,
      55,    56,    56,    56
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     5,     3,     3,     8,     1,     6,     2,     1,
       4,     7,     5,     5,     5,     9,     9,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       1,     1,     4,     4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 2546 "bison.y" /* yacc.c:1646  */
    { insertCommand("HALT"); }
#line 3868 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 2547 "bison.y" /* yacc.c:1646  */
    { insertCommand("HALT"); }
#line 3874 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 2551 "bison.y" /* yacc.c:1646  */
    { insertVariable((yyvsp[0].str)); memIdx ++;}
#line 3880 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 2552 "bison.y" /* yacc.c:1646  */
    { insertArray((yyvsp[-3].str), (yyvsp[-1].str), (yyvsp[-5].str)); memIdx ++;}
#line 3886 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 2553 "bison.y" /* yacc.c:1646  */
    { insertVariable((yyvsp[0].str)); memIdx ++;}
#line 3892 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 2554 "bison.y" /* yacc.c:1646  */
    {insertArray((yyvsp[-3].str), (yyvsp[-1].str), (yyvsp[-5].str)); memIdx ++;}
#line 3898 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 2558 "bison.y" /* yacc.c:1646  */
    { }
#line 3904 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 2559 "bison.y" /* yacc.c:1646  */
    { }
#line 3910 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 2563 "bison.y" /* yacc.c:1646  */
    { assignVariable(); }
#line 3916 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 2564 "bison.y" /* yacc.c:1646  */
    { ifFun(); }
#line 3922 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 2565 "bison.y" /* yacc.c:1646  */
    { ifFun(); }
#line 3928 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 2566 "bison.y" /* yacc.c:1646  */
    { whileFun(); }
#line 3934 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 2567 "bison.y" /* yacc.c:1646  */
    { whileFun(); }
#line 3940 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 2568 "bison.y" /* yacc.c:1646  */
    { endFor((yyvsp[-7].str)); }
#line 3946 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 2569 "bison.y" /* yacc.c:1646  */
    { endForDownTo((yyvsp[-7].str)); }
#line 3952 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 2570 "bison.y" /* yacc.c:1646  */
    { readVariable(); }
#line 3958 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 2571 "bison.y" /* yacc.c:1646  */
    { writeVariable(); }
#line 3964 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 2575 "bison.y" /* yacc.c:1646  */
    { elseFun(); }
#line 3970 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 2578 "bison.y" /* yacc.c:1646  */
    { startWhile(); }
#line 3976 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 2581 "bison.y" /* yacc.c:1646  */
    { startWhile(); }
#line 3982 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 2584 "bison.y" /* yacc.c:1646  */
    { forFrom((yyvsp[0].str)); }
#line 3988 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 2587 "bison.y" /* yacc.c:1646  */
    { forTo(); }
#line 3994 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 2590 "bison.y" /* yacc.c:1646  */
    { forTo(); }
#line 4000 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 2593 "bison.y" /* yacc.c:1646  */
    { forDo(); }
#line 4006 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 2596 "bison.y" /* yacc.c:1646  */
    { forDoDownto(); }
#line 4012 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 2599 "bison.y" /* yacc.c:1646  */
    { }
#line 4018 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 2600 "bison.y" /* yacc.c:1646  */
    { expPlus(); }
#line 4024 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 2601 "bison.y" /* yacc.c:1646  */
    { expMinus(); }
#line 4030 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 2602 "bison.y" /* yacc.c:1646  */
    { expTimes(); }
#line 4036 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 2603 "bison.y" /* yacc.c:1646  */
    { expDivNew(); }
#line 4042 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 2604 "bison.y" /* yacc.c:1646  */
    { expMod(); }
#line 4048 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 2608 "bison.y" /* yacc.c:1646  */
    { condEq(); }
#line 4054 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 2609 "bison.y" /* yacc.c:1646  */
    { condNeq(); }
#line 4060 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 2610 "bison.y" /* yacc.c:1646  */
    { condLE(); }
#line 4066 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 2611 "bison.y" /* yacc.c:1646  */
    { condGE(); }
#line 4072 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 2612 "bison.y" /* yacc.c:1646  */
    { condLEQ(); }
#line 4078 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 2613 "bison.y" /* yacc.c:1646  */
    { condGEQ(); }
#line 4084 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 2617 "bison.y" /* yacc.c:1646  */
    { numIdent((yyvsp[0].str));}
#line 4090 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 2618 "bison.y" /* yacc.c:1646  */
    { }
#line 4096 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 2622 "bison.y" /* yacc.c:1646  */
    { ident((yyvsp[0].str)); }
#line 4102 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 2623 "bison.y" /* yacc.c:1646  */
    { identIdentTab((yyvsp[-3].str), (yyvsp[-1].str)); }
#line 4108 "bison.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 2624 "bison.y" /* yacc.c:1646  */
    { identNumTab((yyvsp[-3].str), (yyvsp[-1].str)); }
#line 4114 "bison.tab.c" /* yacc.c:1646  */
    break;


#line 4118 "bison.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 2627 "bison.y" /* yacc.c:1906  */

void yyerror (char const *s)
{
	fprintf (stderr, "%s\n", s);
	exit(1);
}

int main(int argv, char* argc[]) {
	extern FILE *yyin, *yyout;
	yyin = fopen(argc[1], "r");
	yyout = fopen(argc[2], "w");
    yyparse();
	printCommandsToFile(yyout);
	fclose(yyin); 
}
