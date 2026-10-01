#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pp(int *result, int n){
    for (int i = 0; i < n; i++){
        result[i] = i * i;
    }
}

int main(void) {
    int sq[10];
    pp(sq, 5);
    for (int i = 0; i < 5; i++) {
        printf("%d", sq[i]);
    }
}