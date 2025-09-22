#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int n=1,sayi,toplam=0;
	printf("Bir sayi giriniz: \n");
	scanf("%d" ,&sayi);
	for(int i=1;i<=sayi;i++){
		toplam=toplam+n;
		n++;
	}
	printf("Toplam:%d ", toplam);
	
	return 0;
}