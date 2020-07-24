%{
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

%}
%union {
    char *str;
    long long num;
}
%token <str> DECLARE BGN END ASSIGN IF THEN ELSE ENDIF
%token <str> WHILE DO ENDWHILE ENDDO FOR FROM TO DOWNTO ENDFOR
%token <str> READ WRITE PLUS MINUS TIMES DIV MOD EQ NEQ LE GE LEQ GEQ
%token <str> SEM COM LB RB IDE 
%token <str> NUM
%token <str> COL
%type <str> value
%type <str> identifier
%%
program:
	DECLARE declarations BGN commands END { insertCommand("HALT"); }
	| BGN commands END { insertCommand("HALT"); }
;

declarations:
	declarations COM IDE { insertVariable($3); memIdx ++;}
	| declarations COM IDE LB NUM COL NUM RB { insertArray($5, $7, $3); memIdx ++;}
	| IDE { insertVariable($1); memIdx ++;}
	| IDE LB NUM COL NUM RB {insertArray($3, $5, $1); memIdx ++;}
;

commands:
	commands command { }
	| command { } 
;

command:
	identifier ASSIGN expression SEM { assignVariable(); }
	| IF condition THEN commands _else commands ENDIF { ifFun(); } 
	| IF condition THEN commands ENDIF { ifFun(); }
	| _while condition DO commands ENDWHILE { whileFun(); }
	| _do commands WHILE condition ENDDO { whileFun(); }
	| FOR IDE _forFrom value _forTo value _forDo commands ENDFOR { endFor($2); }
	| FOR IDE _forFrom value _forDownTo value _forDoDownTo commands ENDFOR { endForDownTo($2); }
	| READ identifier SEM { readVariable(); }
	| WRITE value SEM { writeVariable(); }
;

_else:
	ELSE { elseFun(); }

_while:
	WHILE { startWhile(); }

_do:
	DO { startWhile(); } 

_forFrom:
	FROM { forFrom($1); }

_forTo:
	TO { forTo(); }

_forDownTo:
	DOWNTO { forTo(); }

_forDo:
	DO { forDo(); }

_forDoDownTo:
	DO { forDoDownto(); }

expression:
	value { }
	| value PLUS value { expPlus(); }
	| value MINUS value { expMinus(); }
	| value TIMES value { expTimes(); }
	| value DIV value { expDivNew(); }
	| value MOD value { expMod(); }
;

condition:
	value EQ value { condEq(); }
	| value NEQ value { condNeq(); }
	| value LE value { condLE(); }
	| value GE value { condGE(); }
	| value LEQ value { condLEQ(); }
	| value GEQ value { condGEQ(); }
;

value:
	NUM { numIdent($1);}
	| identifier { }
;

identifier:
	IDE { ident($1); }
	| IDE LB IDE RB { identIdentTab($1, $3); } 
	| IDE LB NUM RB { identNumTab($1, $3); } 
;

%%
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
