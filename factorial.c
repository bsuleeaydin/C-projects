#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int fakt=1,i=1,a;
	printf("Bir sayi giriniz: \n");
	scanf("%d", &a);
	for(i=1;i<=a;i++)
	 fakt=fakt*i;
	printf("Faktoriyel: %d",fakt);
	return 0;
}