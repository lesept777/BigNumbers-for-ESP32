#ifndef Utils
#define Utils
#include <Arduino.h>
#include "BigNumbers.h"

/* Utility functions */

// Display help info
void help() ;

// Read the input
char* readSerial ();
bool  parseOperator (char*);
void  parseInput(char*, int = MAXPREC);
void  cleanInput(char*);

#endif