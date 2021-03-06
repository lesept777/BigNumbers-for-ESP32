/* Utility functions */
#include "utils.h"
#include "BigNumbers.h"
uint8_t  excla = 0;
char     oper;

// Display help info
void help() {
  Serial.println("-------------------------------------------------------------------");
  Serial.println("User's manual:");
  Serial.println("A big number is a set of numbers separated by 'e' or 'E'");
  Serial.println("As in engineering format, the 'e' stands for 10^");
  Serial.println("Several consecutive 'e' stand for 10^10^... 10^");
  Serial.println("Enter 2 big numbers and operators (+ * ^) or comparators (< > =");
  Serial.println("Use ! for the factorial. !! is the factorial of the factorial, etc.");
  Serial.println("Some examples:");
  Serial.println("12.345e67!    : factorial of 12.345 10^(67)");
  Serial.println("12e3.4e56 !!  : factorial of factorial of 12 10^(3.4 10^56)");
  Serial.println("3eee23 !      : factorial of 3.10^10^10^23");
  Serial.println("2e3e4 + 3e5e7 : add the 2 big numbers");
  Serial.println("2e3e4 * 3e5e7 : multiply the 2 big numbers");
  Serial.println("2e3e4 ^ 3e5e7 : compute the power of the 2 big numbers (also p or P");
  Serial.println("2e3e4 < 3e5e7 : compare the 2 big numbers");
  Serial.println("-------------------------------------------------------------------");
}

// read and store keyboard input
char* readSerial () {
  static char input[MAXINPUT];
  uint8_t index = 0;
  while (1) {
    if (Serial.available() > 0) {
      input[index] = Serial.read();
      if (input[index] == '\n' || index == MAXINPUT - 1) break;
      ++index;
    }
  }
  while (Serial.available()) Serial.read(); // clear input buffer
  input[index] = 0;
  return input;
}

bool parseOperator (char* input) {
  // search for an operator (><+*=!)
  uint8_t nbOp = 0;
  int index = 0;
  bool stop = false;
  bool found = false;
  while (input[index] != 0 && !stop) {
    switch (input[index]) {
      case '!':
        ++ excla;
        oper = input[index];
        found = true;
        break;
      case '<':
      case '>':
      case '=':
      case '+':
      case '*':
      case 'P':
      case 'p':
      case '^':
        oper = input[index];
        found = true;
        break;
      default:
        if (found) stop = true;
        break;
    }
  ++index;
  }

  if (!found) Serial.println ("found no operator!"); 
  else {
    // if (excla !=0) Serial.printf(" (%d)", excla);
    // Serial.println();
  }

  // Remove parsed characters
  --index;
  uint8_t index0 = index;
  while (input[index] != 0) input[index - index0] = input[index++];
  input[index - index0] = 0;
  return found;
}

// Parse the input string to find the requested operation
void parseInput(char* input, int prec) {
  bigNumber N1, N2, N;
  cleanInput(input);
  if (input[0] == '?' || input[0] == 'h') {help();  return;}
  Serial.printf("> %s\n", input);

  // Search the first number
  N1.parseNumber(input);
  N1.displayStruct(); Serial.println();
  Serial.print("  ");
  N1.displayBigNumber(prec);

  // Search for an operator
  excla = 0;
  bool operat = parseOperator (input);
  if (operat) { // found an operator

    if (oper == '!') {
      uint8_t i = 0;
        while (i++ < excla) Serial.print('!');
        Serial.print (" =");
    } else {
      // Search the second number
      N2.parseNumber(input);
      if (oper == 'p' || oper == 'P') oper = '^';
      Serial.printf("%c ", oper);
      // else Serial.print ("power ");
      N2.displayBigNumber(prec);
    }
    Serial.println();

    // Compute operation
    switch (oper) {
      case '!': {
        uint8_t i = 0;
          while (i++ < excla) {
            N = stirling(N1);
            N1 = N.extract(0);
          }
          N.displayBigNumber(prec);
          break;        
        }
      case '+':
        N = N1 + N2;
        Serial.print("= ");
        N.displayBigNumber(prec);
        break;
      case '*':
        N = N1 * N2;
        Serial.print("= ");
        N.displayBigNumber(prec);
        break;
      case '^':
        N = power(N1, N2);
        Serial.print("= ");
        N.displayBigNumber(prec);
        break;
      case '=':
        Serial.print ((N1 == N2)?"true":"false");
        break;
      case '>':
        Serial.print ((N1 > N2)?"true":"false");
        break;
      case '<':
        Serial.print ((N1 < N2)?"true":"false");
        break;
      default:
        break;
    }
  }
}

// Remove unwanted characters from the string
// Only keep: 0..9 e E ^ p P + = * ! < > .
void cleanInput(char* input) {
  char output[MAXINPUT];
  int i = 0;
  int N = 0;
  while (input[i] != 0) {
    // List of recognized characters:
    switch (input[i]) {
      case '0' ... '9': // Guess what...
      case 'e': // Exponent
      case 'E': // Exponent
      case '^': // Exponent
      case 'p': // power operator
      case 'P': // power operator
      case '<': // Comparison operator
      case '>': // Comparison operator
      case '=': // Comparison operator
      case '+': // Addition
      case '*': // Multiplication
      case '!': // Factorial
      case '.': // Decimal dot
      case 'h': // Display help
      case '?': // Display help
        output[N++] = input[i];
        break;
      default:
        break;
    }
    ++i;
  }
  output[N++] = '\0';
  memcpy (input, output, N);
}