#include <stdio.h>
#include <math.h>

int main() {
    int n, i;
    printf("Bir sayi giriniz:\n");
    scanf("%d", &n);

    if(n <= 1) {
        printf("Sayi asal degildir\n");
        return 0;
    }

    for(i = 2; i <= sqrt(n); i++) {
        if(n % i == 0) {
            printf("%d asal degildir\n", n);
            return 0; // direkt bitiriyoruz
        }
    }

    printf("%d asal sayidir\n", n);
    return 0;
}
