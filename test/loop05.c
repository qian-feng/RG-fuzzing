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
  char input[2][100];
  puts("Please say HI to this popcalc test:\n");
  //  fgets(input, 100, stdin);
  for(int c=0; c<2; c++){
    read(0, input[c], 100);
  }
  
  
  int i = 0, counter1 = 0, counter2=0;
  long long int sum = 0;
  while (i< 100) {
    if (input[0][i] == 'B') {
      counter1++;
    }
    sum += (input[0][i]-'B');
    i++;
  }
  i = 0;
  while(i<sum){
    if (input[1][i] == 'A') {
      counter2++;
    }
    i++;
  }
  if (counter1 == 75 && counter2 == 25) {
    sum = sum/(counter1-75);
    printf("sum: %lld \n", sum);  
  } else {
  sum = sum%(counter1-75);
  printf("sum: %lld \n", sum);  
  }
}

int main() {
  check();
  puts("Bye\n");
  quit();
}
