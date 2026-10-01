#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Widget Widget;

typedef struct {
    void (*render)(Widget *self);   // VTalble에 함수포인터 render, on_event를 선언
    void (*on_event)(Widget *self, int code);           
} VTable;

struct Widget {
    const VTable *vtbl; 
    int id;
    int closed;
    char label[24];
};

#define MAX_WIDGETS 8
typedef struct {
    Widget *items[MAX_WIDGETS]; // items를 8크기의 배열로
    int count;
} Screen;

/* ── 위젯 종류별 동작 ─────────────────────────────────────────── */
static void button_render(Widget *self) {
    printf("  [Button #%d] \"%s\"\n", self->id, self->label);   // render 호출하면 출력하는 코드
}
static void label_render(Widget *self) {
    printf("  Label #%d: %s\n", self->id, self->label);
}
static void dialog_render(Widget *self) {
    printf("  <<Dialog #%d>> %s\n", self->id, self->label);
}

static void widget_noop_event(Widget *self, int code) { (void)self; (void)code; }   // 그냥 더미인듯 | 뭐하는건지 모르겠음

/* 다이얼로그는 이벤트 코드 1(닫기)을 받으면 스스로 정리(파괴)된다 */
static void dialog_on_event(Widget *self, int code);

/* 각 변수에 함수포인터 할당 */
static const VTable BUTTON_VT = { button_render, widget_noop_event };
static const VTable LABEL_VT  = { label_render,  widget_noop_event };
static const VTable DIALOG_VT = { dialog_render, dialog_on_event  };

/* 그냥 새로운 위젯 만드는거 */
static Widget *widget_new(const VTable *vt, int id, const char *label) {

    /* [Thinking Point]
    *   w 에 아직 아무 값도 넣지 않았는데, sizeof *w 로 *w 를 써도 괜찮은 이유는?
    *   tip 1. sizeof 는 피연산자를 '실행(역참조)'하지 않고 '타입'만 본다.
    *          → *w 의 타입(Widget)만 필요할 뿐, w 를 실제로 따라가지 않는다.
    *   tip 2. 그래서 sizeof *w 는 (VLA 제외) 컴파일 타임에 sizeof(Widget) 상수로 치환된다.
    *   생각해보기: sizeof(Widget) 대신 sizeof *w 로 쓰면 어떤 장점이 있을까?
    */
    Widget *w = malloc(sizeof *w);  // 받은 사이즈를 *w에
    if (!w) { perror("malloc"); exit(1); }
    w->vtbl = vt;
    w->id = id;
    w->closed = 0;
    strncpy(w->label, label, sizeof(w->label) - 1);
    w->label[sizeof(w->label) - 1] = '\0';
    return w;
}

/* 호출되면 해당 위젯 프리 */
static void widget_destroy(Widget *w) {
    free(w);                // 얘가 문제
}

/* ── Screen ──────────────────────────────────────────────────── */
static void screen_add(Screen *s, Widget *w) {
    if (s->count < MAX_WIDGETS) s->items[s->count++] = w;
}

/* for문 돌리면서 위젯을 s->items[i]번째 값으로 선언하고 on_event호출 */
static void screen_dispatch(Screen *s, int code) {
    for (int i = 0; i < s->count; i++) {
        Widget *w = s->items[i];
        w->vtbl->on_event(w, code);
    }
}

/* 위 처럼 위젯에 저장하고 출력 (print) */
static void screen_render(Screen *s) {
    for (int i = 0; i < s->count; i++) {
        if (s->items[i] == NULL) {
            continue;
        }
        Widget *w = s->items[i];
        w->vtbl->render(w);      // 없음
    }
}

/* 전체 코드를 뜯어보면 함수 포인터 선언하는 과정에서 dialog만 on_event가 다름 */
static void dialog_on_event(Widget *self, int code) {
    if (code == 1) {
        self->closed = 1;
    }
}

static char *app_build_status(const char *text) {
    char *msg = malloc(sizeof(Widget));   
    if (!msg) exit(1);

    /* [테스트용 연출] 재사용한 메모리를 0xAB 로 '일부러' 덮어써서 오염시킨다.
     * 실무라면 다른 기능이 우연히 이 자리를 덮어쓰겠지만, 여기서는 UAF 크래시를
     * 매번 똑같이(결정적으로) 재현하기 위해 인위적으로 채운다. 
     * glibc(리눅스) 환경 (tcache)에서만 유효하다. 환경&상황에 따라 msg는 새로운 주소로 할당될 수 있다.
     */
    memset(msg, 0xAB, sizeof(Widget));
    snprintf(msg, sizeof(Widget), "STATUS: %s", text);
    return msg;
}

int main(void) {
    Screen s = { .count = 0 };

    /* 그냥 텍스트 출력 코드임 */
    screen_add(&s, widget_new(&LABEL_VT,  10, "Welcome"));
    screen_add(&s, widget_new(&BUTTON_VT, 11, "OK"));
    screen_add(&s, widget_new(&DIALOG_VT, 12, "Are you sure?"));  /* items[2] */
    screen_add(&s, widget_new(&BUTTON_VT, 13, "Cancel"));

    printf("frame 1:\n");
    screen_render(&s);
    screen_dispatch(&s, 1);

    /* TODO 닫힌(closed) 위젯을 여기서 정리(free + 해당 슬롯 NULL)할 필요가 있음 */
    for (int i = 0; i < s.count; i++){
        if (s.items[i] != NULL && s.items[i]->closed == 1){
            widget_destroy(s.items[i]);
            s.items[i] = NULL;
        }
    }
    

    char *status = app_build_status("dialog closed");
    printf("%s\n", status);
    printf("frame 2:\n");
    screen_render(&s);     // 여기서 2일 때 segfault      

    free(status);

    for (int i = 0; i < s.count; i++) free(s.items[i]);
    return 0;
}