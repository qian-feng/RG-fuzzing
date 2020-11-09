#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

void quit() {                                                                                                                     
 	exit(0);
}

void check() {
  char input[100];
  puts("Please say HI to this popcalc test:\n");
  //	fgets(input, 100, stdin);
  read(0, input, 100);
  int i = input[0], counter = 0;
  long long int sum = 0;
  while (i++%100!=0) {
    if (input[i%100] == 'B') {
      counter++;
    }
    sum += (input[i%100]-'A');
  }

  if (counter == 75) {
	sum = sum/(counter-75);
 	printf("sum: %lld \n", sum);	
  } else {
	sum = sum%(counter-75);
 	printf("sum: %lld \n", sum);	
  }
}

int main(int argc, char *argv[]) {
  check();
  puts("Bye\n");
  quit();
}
