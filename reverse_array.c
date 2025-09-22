#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int dizi[5],i,j;
	printf("diziyi girin: ");
	for(int n=0;n<5;n++){
		scanf("%d" , &dizi[n]);
	}
	
	i=0;
	j=4;
	while(i<j){
		int temp=dizi[i];
		dizi[i]=dizi[j];
		dizi[j]=temp;
		i++;
		j--;
	}
	
	for(int n=0;n<5;n++){
		printf("%d",dizi[n]);
	}
	
	return 0;
}