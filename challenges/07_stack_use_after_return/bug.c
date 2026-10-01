#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 8
typedef struct {
    char **lines;    /* 줄 포인터들의 '배열'을 가리킨다 */
    int    count;
} LineView;

/* 결과를 뷰에 채운다(포인터를 함수 경계 너머로 옮겨 -Wdangling 을 회피하는 형태) */
static void view_set(LineView *out, char **arr, int n) {
    out->lines = arr;
    out->count = n;
} 
static void split_lines(LineView *out, char *text) {            
    int n = 0;
    /* 주소를 여러개 저장하므로 배열 한 칸의 크기를 포인터 크기로 | [char *]들(포인터 배열)을 가리키기에 이중 포인터 */
    char **parts = malloc(MAX_LINES * sizeof(char *));
    /* strtok는 새로 할당하지 않고, 넘겨받은 문자열 내부의 주소를 돌려준다. 
    * 따라서, strtok은 원본 버퍼를 제자리에서 수정한다. 
    */
    for (char *ln = strtok(text, "\n"); ln && n < MAX_LINES; ln = strtok(NULL, "\n"))
        parts[n++] = ln;

    view_set(out, parts, n);      

    /* TODO 상기 코드를 수정하여 결과를 호출자가 준 out 에 직접 채운다(값 반환 아님, 지역 주소 반환 아님). */       
}

/* split_lines 가 쓰던 스택 프레임을, 같은 모양(char*[8])의 지역 배열로 덮는다.
   무효가 된 parts[] 자리에 '그럴듯한 쓰레기 포인터'가 들어차게 만든다. */
static void warm_stack(void) {
    char *scratch[MAX_LINES];
    for (int i = 0; i < MAX_LINES; i++)
        scratch[i] = (char *)0x4141414141414141ULL;   /* 매핑되지 않은 주소 */
    __asm__ volatile("" :: "r"(scratch) : "memory");   /* 최적화 제거 방지 */
}

int main(void) {
    char text[] = "alpha\nbeta\ngamma";

    LineView v;
    split_lines(&v, text);               
    warm_stack();          

    long checksum = 0;
    for (int i = 0; i < v.count; i++)
        checksum += (unsigned char)v.lines[i][0];

    printf("lines = %d, checksum = %ld\n", v.count, checksum);
    free(v.lines);
    return 0;
}