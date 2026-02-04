#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int n, i, j, k, s;
	printf("Bir sayi giriniz: ");
	scanf("%d", & n);
	if(n>=1 && n<=9){
		for(i = 1; i<=n; i++){
			for(j=1; j<= n-i; j++){
				printf("-");}
				s=n;
				for(k=0; k<=n-j;k++){
					printf("%d", s);
					s--;
			}
			printf("\n");
		}
	}
	
	
	return 0;
}