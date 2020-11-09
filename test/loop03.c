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
  char input[100][100];
  puts("Please say HI to this popcalc test:\n");
  //  fgets(input, 100, stdin);
  for(int c=0; c<100; c++)
  {
    read(0, input[c], 100);
  }
  
  int i = 0, j, counter = 0;
  long long int sum = 0;
  while (i< 100) {
    j = 0;
    while (j < 100) {
        if (input[i][j] == 'B') {
            counter++;
        }
        if (input[i][j] == 'C'){
            counter--;
        }
        j++;
        sum += (input[i]-'A');
    }
    i++;
  }

  if (counter == 75) {
      sum = sum/(sum-75);
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
