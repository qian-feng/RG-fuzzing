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
  int sum, d, i, counter=0;
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
  i=0;
  while(i < d){
    if(arr1[i] == arr2[i]){
        counter++;
    }
    sum++;
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