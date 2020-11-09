#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {   // 1
  if(atoi(argv[1]) == 5) {
    printf("if-branch 1\n");        // 2
    printf("if-branch 1\n");
    printf("if-branch 1\n");
  }
  else {
    printf("else-branch 1\n");      // 3
    printf("else-branch 1\n");    
    printf("else-branch 1\n");   
  }

  if(atoi(argv[1]) == 6) {          // 4
    printf("if-branch 2\n");        // 5
    printf("if-branch 2\n");
    printf("if-branch 2\n");   
  }
  else {
    printf("else-branch 2\n");      // 6
    printf("else-branch 2\n"); 
    printf("else-branch 2\n");  
  }
  return 0;                         // 7 - total
}                                   