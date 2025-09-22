#include <stdio.h>

int main() {
    int dizi[10], max, min, i;

    printf("10 elemanlı dizi girin:\n");
    for(i = 0; i < 10; i++) {
        printf("%d. sayi: ", i + 1);
        scanf("%d", &dizi[i]);
    }

    // Başlangıç değerleri
    min = max = dizi[0]; //önemli olan bu atamayı yapmak yoksa dngüde sadece kendşnden öncekini kıyaslar

    for(i = 1; i < 10; i++) {
        if(dizi[i] < min)
            min = dizi[i];
        if(dizi[i] > max)
            max = dizi[i];
    }

    printf("Dizinin min: %d max: %d'dir\n", min, max);
    return 0;
}
