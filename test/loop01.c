#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <klee/klee.h>

void quit() {                                                                                                                     
 	exit(0);
}

void check() {
  char input[100];
  //puts("Please say HI to this popcalc test:\n");
  //	fgets(input, 100, stdin);
  klee_make_symbolic(input, sizeof input, "in");
	
  int i = 0, counter = 0;
  long long int sum = 0;
  while (i++ < 100) {
    if (input[i] == 'B') {
      counter++;
    }
    sum += (input[i]-'A');
  }

  if (counter == 75) {
	   sum = sum/(counter-75);
 	   //printf("sum: %lld \n", sum);	
  } else {
	   sum = sum%(counter-75);
 	    //printf("sum: %lld \n", sum);	
  }
}

int main() {
  check();
  puts("Bye\n");
  quit();
}
