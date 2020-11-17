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
  //  fgets(input, 100, stdin);   
  int i = -1,  counter = 0;
  char j;
  long long int sum = 0;
  read(0, input, 100); 
  while (i++ < 100) {
        j = 'B' + i%10;
        if (input[i] == j) {
            counter++;
        }
        sum += (input[i]-'A');
  }  
  // 0 is the index of the edge! I need AA to fulfill this format
  AIF_SET(0, counter);
  AIF_RANGE(0, counter, 75, 75);
  if (counter == 75) {
      sum = sum/(counter-75);
      printf("sum: %lld \n", sum);  
  } else {
      sum = sum%(counter-75);
      printf("sum: %lld \n", sum);  
  }
}

int main() {
  check();
  puts("Bye\n");
  quit();
}
