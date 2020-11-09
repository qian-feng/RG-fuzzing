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
  int cn1, cn2, cn3;
  puts("Please say HI to this popcalc test:\n");
  scanf("%d%d%d", &cn1, &cn2, &cn3);
  int i=0, x=0, y=0, sum;
  if(cn1 > 50000 || cn2 > 500000 ||  cn3 > 50000) return;
  while(i++<cn3){
    if(i>cn1) x += cn1*2 + cn3;
    if(i>cn2) y += cn2*3 + cn3;
    else{
      x = x+2;
      y = x+1;
    }
    sum++;
  }
  if(x==y){
        sum = sum/(cn2-75);
  }
}

int main() {
  check();
  puts("Bye\n");
  quit();
}


