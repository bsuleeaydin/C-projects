#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int n=1;
	while(n<=100){
		if(n%2==0)
		   printf("\n%d",n);
		n++;
	}	
	return 0;
}