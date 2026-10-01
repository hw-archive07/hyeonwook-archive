#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 필요한 총 바이트 수 = 모든 조각 길이 합 + 종료 문자 1 */
static size_t joined_size(const char *const *parts, int n) {
    size_t total = 1;                        /* '\0' 자리 */
    for (int i = 0; i < n; i++) {     // 바꿈 i < n - 1   
        total += strlen(parts[i]);        // n - 1로 했을 때 total은 -1만큼 덜 더해짐
    }
    return total;
}

static char *join(const char *const *parts, int n) {
    size_t need = joined_size(parts, n);
    char *out = malloc(need);                /* 마지막 조각 길이만큼 부족하게 할당됨 */
    if (!out) { perror("malloc"); exit(1); }

    size_t off = 0;
    for (int i = 0; i < n; i++) {            /* 복사는 마지막 조각까지 전부 → 오버플로 */
        strcpy(out + off, parts[i]);    // 그런데 적은 값이 들어간 상태에서 기존 값 만큼 쓰려니까 오류남
        off += strlen(parts[i]);
    }
    out[off] = '\0';
    return out;
}

int main(void) {
    
    static char body[200000];
    memset(body, 'x', sizeof body - 1);
    body[sizeof body - 1] = '\0';

    const char *parts[] = { "GET ", "/index.html", " HTTP/1.1\r\n\r\n", body };
    int n = (int)(sizeof(parts) / sizeof(parts[0]));

    char *msg = join(parts, n);              /* 복사 중 힙 오버플로 → 크래시 */

    printf("joined length = %zu\n", strlen(msg));
    free(msg);
    return 0;
}
