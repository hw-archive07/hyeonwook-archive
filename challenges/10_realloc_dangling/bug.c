#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_UNDO 8
typedef struct {
    int   *data;
    size_t len, cap;
    int   *clipboard;       
    int   *undo[MAX_UNDO];   
    int    undo_n;
} EditBuffer;

static void eb_init(EditBuffer *e) {
    e->cap = 4;
    e->len = 0;
    e->undo_n = 0;
    e->data = malloc(e->cap * sizeof(int));
    if (!e->data) { perror("malloc"); exit(1); }
    /* data 바로 뒤에 놓이는 별도 할당. data 가 힙 맨 끝(top)이 아니게 되어
       이후 realloc 이 제자리 확장 대신 '이동'을 택하게 만든다(→ 옛 블록 해제). */
    e->clipboard = malloc(e->cap * sizeof(int));
    if (!e->clipboard) { perror("malloc"); exit(1); }
}

static void eb_snapshot(EditBuffer *e) {
    if (e->undo_n < MAX_UNDO){
        int *backup = malloc(e->cap * sizeof(int)); // 다음 realloc으로 인한 오류를 막기 위해 백업
        memcpy(backup, e->data, sizeof(int) * e->len); // 깊은복사
        e->undo[e->undo_n++] = backup;
    }
}

static void eb_grow(EditBuffer *e, size_t need) {
    size_t nc = e->cap;
    while (nc < need) nc *= 2;
    int *p = realloc(e->data, nc * sizeof(int)); // realloc은 size바이트 만큼 연속된 메모리를 할당할 수 없을 경우
    if (!p) { perror("realloc"); exit(1); }      // 새로운 영역을 할당 후 기존 요소들을 복사하여 새 메모리 주소를 반환한다.
    e->data = p;                                 // **(엄청난 사실)**
    e->cap = nc;                                
}

static void eb_push(EditBuffer *e, int v) {
    if (e->len == e->cap) eb_grow(e, e->len + 1);
    e->data[e->len++] = v;
}

static void eb_free(EditBuffer *e) {
    free(e->data);
    free(e->clipboard);
    for (int i = 0; i < e->undo_n; i++) {
        if (e->undo[i] != NULL){
            free(e->undo[i]); 
        }          
    }
    e->undo_n = 0;
    e->data = NULL;
}

int main(void) {
    EditBuffer e;
    eb_init(&e);

    for (int i = 0; i < 3; i++) eb_push(&e, i);

    eb_snapshot(&e);                 

    for (int i = 0; i < 4000; i++) eb_push(&e, i);     

    printf("len=%zu cap=%zu head=%d tail=%d\n",
           e.len, e.cap, e.data[0], e.data[e.len - 1]);

    eb_free(&e);                     
    printf("done\n");
    return 0;
}
