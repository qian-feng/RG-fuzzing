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
  //	fgets(input, 100, stdin);
  for(int c=0; c<2; c++){
    read(0, input[c], 100);
  }
	int i=0, j=0, counter=0, sum;
  while(i<100 && j<100){
    if(input[0][i] == input[1][j]) {
      counter++;
      i++;
      j++;
    }
    else{
      i = i-j+1;
      j = 0;
    }
    sum++;

  }
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
