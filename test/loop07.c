#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

void quit() {                                                                                                                     
    exit(0);
}

int check() {
  int sum, d, i, j, k;
  puts("Please say HI to this popcalc test:\n");
  //    fgets(input, 100, stdin);
  scanf("%d", &d);;
  char * arr1 = malloc(sizeof(char) * d);
  if(!arr1) return 0;
  for (i=0; i<d; ++i){
        scanf("%c",&arr1[i]);
  }
  char * arr2 = malloc(sizeof(char) * d);
  if(!arr2) return 0;
  for (i=0; i<d; ++i){
        scanf("%c",&arr2[i]);
  }
  char * output = malloc(d);
  if(output){
      for (i = 0; i < d; i++)
          output[i] = arr1[j] + arr2[k];
  }
  if (output[d%10-1] == 'D') {
        sum = 50/(output[d%15-2]-'C');
        printf("sum: %d \n", sum); 
  }   
}

int main() {
  check();
  puts("Bye\n");
  quit();
}

