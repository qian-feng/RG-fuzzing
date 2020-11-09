
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
  int sum=0, d, i, m, x=0;
  puts("Please say HI to this popcalc test:\n");
  //    fgets(input, 100, stdin);
  scanf("%d%d", &d,&m);
  char * arr1 = malloc(sizeof(char) * d);
  if(!arr1) return 0;
  for (i=0; i<d; ++i){
        scanf("%c",&arr1[i]);
  }
  i = 0;
  while(i < d){
    if(i>m) x = i+2;
    else
    	x =i*(m%d);
    i++;
  }
  if (arr1[x%d] == 'B') {
      sum = sum/(i-d);
  }  
}

int main() {
  check();
  puts("Bye\n");
  quit();
}