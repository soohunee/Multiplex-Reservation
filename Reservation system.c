#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX 1000

int cnt = 0;

typedef enum { false, true } bool;

typedef struct Node { //node의 구성
    int rsvNum;
    int rsvDate;
    int rsvTime;
    int rsvMovie;
    int rsvSeat;
    char payCard;
    char color;
    struct Node* right;
    struct Node* left;
    struct Node* parent;
}Node;

typedef struct queue_ { //트리가 제대로 생성됐나 확인하기 한 printTree를 위한 queue(실제 프로그램에서는 쓰지 않음)
    int front;
    int rear;
    int count;
    Node* items[MAX];
}Queue;

typedef struct Tree { // tree의 구성 root와 nil node
    Node* root;
    Node* NIL;
}Tree;

Node* createNode(int rsvNum, int rsvDate, int rsvTime, int rsvMovie, int rsvSeat, char payCard) { // tree에 insert하거나 delete하기 위해 node를 만드는 함수
    Node* n = malloc(sizeof(Node));
    n->left = NULL;
    n->right = NULL;
    n->parent = NULL;
    n->rsvNum = rsvNum;
    n->rsvDate = rsvDate;
    n->rsvTime = rsvTime;
    n->rsvMovie = rsvMovie;
    n->rsvSeat = rsvSeat;
    n->payCard = payCard;
    n->color = 'r';

    return n;
}

Tree* initTree() { // 처음 tree를 초기화시키는 함수
    Tree* t = malloc(sizeof(Tree));
    Node* nil_node = malloc(sizeof(Node));
    nil_node->left = NULL;
    nil_node->right = NULL;
    nil_node->parent = NULL;
    nil_node->color = 'b';
    nil_node->rsvNum = 0;
    t->NIL = nil_node;
    t->root = t->NIL;
    return t;
}

void left_rotate(Tree* t, Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != t->NIL) { //y의 왼쪽 자식이 존재하는 경우
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == t->NIL) { // root라는 의미
        t->root = y;
    }
    else if (x == x->parent->left) { //x가 왼쪽자식인 경우
        x->parent->left = y;
    }
    else {  //x가 오른쪽 자식인 경우
        x->parent->right = y; 
    }
    y->left = x;
    x->parent = y;
}

void right_rotate(Tree* t, Node* x) {
    Node* y = x->left;
    x->left = y->right;
    if (y->right != t->NIL) { //y의 오른쪽 자식이 존재하는 경우
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == t->NIL) {  //root라는 의미
        t->root = y;
    }
    else if (x == x->parent->right) {   //x가 오른쪽 자식인 경우
        x->parent->right = y;
    }
    else {  //x가 왼쪽 자식인 경우
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}


void rbInsertFixup(Tree* t, Node* z) {  //insert하고 rbtree property를 만족시켜주기 위함
    while (z->parent->color == 'r') {   //부모가 r라면 삽입된 노드도 r이므로 property 위반
        if (z->parent == z->parent->parent->left) { //  부모가 할아버지 노드의 왼쪽 자식인 경우

            Node* y = z->parent->parent->right;

            if (y->color == 'r') {
                z->parent->color = 'b';
                y->color = 'b';
                z->parent->parent->color = 'r';
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(t, z);
                }
                z->parent->color = 'b';
                z->parent->parent->color = 'r';
                right_rotate(t, z->parent->parent);
            }
        }
        else {  //  부모가 할아버지 노드의 왼쪽 자식인 경우
            Node* y = z->parent->parent->left;

            if (y->color == 'r') {
                z->parent->color = 'b';
                y->color = 'b';
                z->parent->parent->color = 'r';
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(t, z);
                }
                z->parent->color = 'b';
                z->parent->parent->color = 'r';
                left_rotate(t, z->parent->parent);
            }
        }
    }
    t->root->color = 'b';
}
void rbInsert(Tree* t, int rsvNum, int rsvDate, int rsvTime, int rsvMovie, int rsvSeat, char payCard) { //삽입 함수
    Node* z;
    z = createNode(rsvNum, rsvDate, rsvTime, rsvMovie, rsvSeat, payCard);
    Node* y = t->NIL;
    Node* temp = t->root;

    while (temp != t->NIL) {
        y = temp;
        if (z->rsvNum < temp->rsvNum)
            temp = temp->left;
        else
            temp = temp->right;
    }
    z->parent = y;

    if (y == t->NIL) {
        t->root = z;
    }
    else if (z->rsvNum < y->rsvNum)
        y->left = z;
    else
        y->right = z;

    z->right = t->NIL;
    z->left = t->NIL;   //일단 BST의 insert와 동일(NULL이 아닌 t->NIL인 것이 차이점)

    rbInsertFixup(t, z);
}

void transplantNode(Tree* t, Node* u, Node* v) {    //delete 하는 과정에서 node를 갈아 끼우기 위한 함수
    if (u->parent == t->NIL)
        t->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

Node* minimum(Tree* t, Node* x) {   //delete 하는 과정에서 삭제할 노드의 오른쪽 subtree의 최소값을 찾기 위한 함수
    while (x->left != t->NIL)
        x = x->left;
    return x;
}

void rbDeleteFixup(Tree* t, Node* x) {  //삭제되는 노드가 b일 경우 rbtree property를 위반하지 않기 위함
    Node* w;
    while (x != t->root && x->color == 'b') {
        if (x == x->parent->left) {
            w = x->parent->right;
            if (w->color == 'r') {
                w->color = 'b';
                x->parent->color = 'r';
                left_rotate(t, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == 'b' && w->right->color == 'b') {
                w->color = 'r';
                x = x->parent;
            }
            else {
                if (w->right->color == 'b') {
                    w->left->color = 'b';
                    w->color = 'r';
                    right_rotate(t, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 'b';
                w->right->color = 'b';
                left_rotate(t, x->parent);
                x = t->root;
            }
        }
        else {
            w = x->parent->left;
            if (w->color == 'r') {
                w->color = 'b';
                x->parent->color = 'r';
                right_rotate(t, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == 'b' && w->left->color == 'b') {
                w->color = 'r';
                x = x->parent;
            }
            else {
                if (w->left->color == 'b') {
                    w->right->color = 'b';
                    w->color = 'r';
                    left_rotate(t, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 'b';
                w->left->color = 'b';
                right_rotate(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = 'b';
}

Node* findNode(Node* n, int value) { // 해당 값의 node를 찾는 함수 - 이 프로그램에서 값은 예약번호에 해당
    if (value < n->rsvNum) {
        if (n->left == NULL) {
            return NULL;
        }
        return findNode(n->left, value);
    }
    else if (value > n->rsvNum) {
        if (n->right == NULL) {
            return NULL;
        }
        return findNode(n->right, value);
    }
    else {
        return n;
    }
}

void rbDelete(Tree* t, int rsvNum) {    //특정 node를 삭제하는 함수
    if (t->root->left == t->NIL && t->root->right == t->NIL && t->root->parent == t->NIL) {
        t->root = t->NIL;
    }
    else {
        Node* z, * x, * y;
        z = findNode(t->root, rsvNum);
        y = z;
        char y_ogclr = y->color;
        if (z->left == t->NIL) {
            x = z->right;
            transplantNode(t, z, z->right);
        }
        else if (z->right == t->NIL) {
            x = z->left;
            transplantNode(t, z, z->left);
        }
        else {
            y = minimum(t, z->right);
            y_ogclr = y->color;
            x = y->right;
            if (y->parent == z) {
                x->parent = y;
            }
            else {
                transplantNode(t, y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplantNode(t, z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        if (y_ogclr == 'b')
            rbDeleteFixup(t, x);
    }
}

void inorder(Tree* t, Node* n) {    //rbtree를 중위순회하며 print하는 함수 - debugging용
    if (n != t->NIL) {
        inorder(t, n->left);
        if (n->color == 'r') {
            printf("%d[R] ", n->rsvNum);
        }
        else if (n->color == 'b') {
            printf("%d[B] ", n->rsvNum);
        }
        inorder(t, n->right);
    }
}

int height(Tree* t, Node* n) {  //트리의 높이를 알아내는 함수
    if (n == t->NIL)
        return 0;
    else {
        int left = height(t, n->left);
        int right = height(t, n->right);
        if (left <= right)
            return right + 1;
        else
            return left + 1;
    }
}

void enqueue(Queue* q, Node* new) {     //enqueue
    q->items[q->rear] = new;
    q->rear++;
    q->count++;
}

Node* dequeue(Queue* q) {      //dequeue
    Node* temp;
    temp = q->items[q->front];
    q->front++;
    q->count--;
    return temp;
}

void printTree(Tree* t, Queue* q) {     //트리를 레벨 별로 나타내는 함수 - debugging용
    Node* nl;
    nl = createNode(-1, 0, 0, 0, 0, 0);
    Node* temp;
    int cnt1 = 0;
    int cnt2 = 0;
    int h = height(t, t->root);
    if (t->root == NULL) {
        return;
    }
    enqueue(q, t->root);
    cnt1++;
    for (int i = 0; i < h; i++) {
        while (cnt1 != 0) {
            temp = dequeue(q);
            cnt1--;
            if (temp->rsvNum == -1) {
                printf("N ");
            }
            else {
                if (temp != NULL) {
                    printf("%d[%c]", temp->rsvNum - 10000, temp->color);
                }
                if (temp->left == t->NIL) {
                    enqueue(q, nl);
                }
                else {
                    enqueue(q, temp->left);
                }
                if (temp->right == t->NIL) {
                    enqueue(q, nl);
                }
                else {
                    enqueue(q, temp->right);
                }
                cnt2 += 2;
            }
        }
        printf("\n");
        cnt1 = cnt2;
        cnt2 = 0;
    }
}
void countNodes(Tree* t, Node* n) { //트리의 총 노드 수를 세는 함수 - inorder
    if (n != t->NIL) {
        countNodes(t, n->left);
        cnt++;
        countNodes(t, n->right);
    }
}
void randomInitSeat(int* A) {
    A[0] = rand() % 100 + 1;
    for (int i = 0; i < 33; i++) {
        A[i] = rand() % 100 + 1;
        for (int j = 0; j < i; j++) {
            if (A[i] == A[j])
                i--;
        }
    }
}
void randomInitPay(int* P) {    //1~2 난수 33개
    for (int i = 0; i < 33; i++) {
        P[i] = rand() % 2 + 1;
    }
}
void randomInitBase(int* A, int* P) {
    randomInitSeat(A); //33개의 1~100 난수
    randomInitPay(P); // 33개의 1~2 난수
}
void inorderLeaves(Tree* t, Node* n) {  //트리를 중위순회하며 leaf들의 정보를 print하는 함수
    if (n != t->NIL) {
        inorderLeaves(t, n->left);
        if (n->left == t->NIL && n->right == t->NIL) {
            printf("Reservation number : %d\n", n->rsvNum);
            printf("Date : %d\n", n->rsvDate);
            printf("Movie : %d\n", n->rsvMovie);
            printf("Time : %d\n", n->rsvTime);
            printf("Pay by card : %c\n\n", n->payCard);
        }
        inorderLeaves(t, n->right);
    }
}
void printRootLeaves(Tree* t) { //트리의 root 정보를 나열한 후, 트리를 주우이순회하며 leaf들의 정보를 print하는 함수
    printf("key value of root : \n");
    printf("Reservation number : %d\n", t->root->rsvNum);
    printf("Date : %d\n", t->root->rsvDate);
    printf("Movie : %d\n", t->root->rsvMovie);
    printf("Time : %d\n", t->root->rsvTime);
    printf("Pay by card : %c\n", t->root->payCard);
    printf("\n\n");
    printf("Leaves (from left to right) : \n");
    inorderLeaves(t, t->root);
}
void initQueue(Queue* queue) {  //queue 초기화
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
}
void randomInit(int* A, int* P, Tree* day1_t, Tree* day2_t, Tree* day3_t, int number[], int rsvNum) {   //각 영화 별로 33%의 좌석을 랜덤하게 예약하는 함수
    randomInitBase(A, P);
    for (int d = 0; d < 3; d++) {
        for (int n = 0; n < 3; n++) {
            for (int k = 0; k < 3; k++) {
                for (int i = 0; i < 33; i++) {
                    if (d == 0) {
                        if (P[i] == 1) {
                            rbInsert(day1_t, number[rsvNum], 1, n + 1, k + 1, A[i], 'y');
                            rsvNum++;
                        }
                        else if (P[i] == 2) {
                            rbInsert(day1_t, number[rsvNum], 1, n + 1, k + 1, A[i], 'n');
                            rsvNum++;
                        }
                    }
                    else if (d == 1) {
                        if (P[i] == 1) {
                            rbInsert(day2_t, number[rsvNum], 2, n + 1, k + 1, A[i], 'y');
                            rsvNum++;
                        }
                        else if (P[i] == 2) {
                            rbInsert(day2_t, number[rsvNum], 2, n + 1, k + 1, A[i], 'n');
                            rsvNum++;
                        }
                    }
                    else if (d == 2) {
                        if (P[i] == 1) {
                            rbInsert(day3_t, number[rsvNum], 3, n + 1, k + 1, A[i], 'y');
                            rsvNum++;
                        }
                        else if (P[i] == 2) {
                            rbInsert(day3_t, number[rsvNum], 3, n + 1, k + 1, A[i], 'n');
                            rsvNum++;
                        }
                    }
                }
                randomInitBase(A, P);
            }
        }
    }
}
void printSeatLayout(int* seatLayout) { //좌석 정보를 print하는 함수
    printf("\n\n                                            [ S C R E E N ]\n\n\n");
    for (int i = 0; i < 5; i++) {
        printf("    ");
        for (int j = (i * 20); j < (i * 20) + 20; j++) {
            if (seatLayout[j] == 0) {
                printf("[XX] ");
            }
            else {
                printf("[%02d] ", seatLayout[j]);
            }
        }
        printf("\n\n");
    }
    for (int i = 0; i < 100; i++) {
        seatLayout[i] = i + 1;
    }
}
void inorderSeat(Tree* t, Node* n, int movie, int times, int* seatLayout) { //좌석 정보에 에약된 좌석을 미리 표시해두는 함수
    if (n != t->NIL) {
        if (n->rsvMovie == movie && n->rsvTime == times) {
            seatLayout[n->rsvSeat - 1] = 0;
        }
        inorderSeat(t, n->left, movie, times, seatLayout);
        inorderSeat(t, n->right, movie, times, seatLayout);
    }
}
void printMovie(int n) {    //번호에 따른 영화 리스트 프린트
    if (n == 1) {
        printf("Movie : About time\n");
    }
    if (n == 2) {
        printf("Movie : The Avengers\n");
    }
    if (n == 3) {
        printf("Movie : Frozen\n");
    }
}
void printTimes(int n) {    //번호에 따른 시간 리스트 프린트
    if (n == 1) {
        printf("Time : 12 : 00\n");
    }
    if (n == 2) {
        printf("Time : 15 : 00\n");
    }
    if (n == 3) {
        printf("Time : 18 : 00\n");
    }
}
void printDate(int n) { //번호에 따른 일자 리스트 프린트
    if (n == 1) {
        printf("Date : 01.12.2020\n");
    }
    if (n == 2) {
        printf("Date : 02.12.2020\n");
    }
    if (n == 3) {
        printf("Date : 02.12.2020\n");
    }
}
void printPay(char n) { //문자에 따른 결제 방법 프린트
    if (n == 'y') {
        printf("Pay : Card\n");
    }
    else {
        printf("Pay : Cash\n");
    }
}
void printResult(int rsvNum, int date, int times, int seat, int movie, char pay) {  //예약확정 결과 프린트
    printf("Reservation Number : %d\n", rsvNum);
    printDate(date);
    printMovie(movie);
    printTimes(times);
    printf("Seat Number : %d\n", seat);
    printPay(pay);
}
void UI(Tree* day1_t, Tree* day2_t, Tree* day3_t, int* seatLayout, int number[]) {  // UI를 나타내는 함수
    int first, date, times, movie, seat, choice, cancelRsvNum, cancelDate;
    char payCard;
    int rsvNum = 890;

    while (1) { 
        rsvNum++;
        printf("**********************\n");
        printf("1. Make a reservation\n");
        printf("2. Cancel a reservation\n");
        printf("3. Exit\n");
        printf("**********************\n");
        printf("Enter your choice : ");
        scanf("%d", &first);
        system("cls");
        if (first == 1) {   // 예매하는 기능
            printf("*****************\n");
            printf("1 : 01.12.2020\n");
            printf("2 : 02.12.2020\n");
            printf("3 : 03.12.2020\n");
            printf("*****************\n");
            printf("Select the date : ");   //날짜 선택
            scanf("%d", &date);
            system("cls");
            printf("******************************************************\n");
            printf("1. About time      2. The Avengers        3. Frozen\n");
            printf("1.   12:00               12:00               12:00\n");
            printf("2.   15:00               15:00               15:00\n");
            printf("3.   18:00               18:00               18:00\n");
            printf("******************************************************\n");
            printf("Select the movie number : ");   // 영화 선택
            scanf("%d", &movie);
            printf("Select the time number : ");    // 시간 선택
            scanf("%d", &times);
            system("cls");
            if (date == 1) {    //01.12.2020 에 예매
                inorderSeat(day1_t, day1_t->root, movie, times, seatLayout);
                printSeatLayout(seatLayout);
                printf("Please select the seat number : "); //좌석 석택
                scanf("%d", &seat);
                printf("Will you pay by card ? (if yes enter 'y', if no enter 'n') : ");
                scanf(" %c", &payCard); // 결제 방법 선택
                system("cls");
                printf("Your reservation has been made successfully !!\n");
                printf("******************************************************\n");
                printResult(number[rsvNum], date, times, seat, movie, payCard); //예약 결과 프린트
                printf("******************************************************\n");
                rbInsert(day1_t, number[rsvNum], 1, times, movie, seat, payCard);   //tree에 예약 노드 insert
                printRootLeaves(day1_t);    //root, leaves 정보 나열
                countNodes(day1_t, day1_t->root);   
                printf("Total number of nodes : %d", cnt); //tree의 총 노드수 프린트
                cnt = 0;
                printf("\n\nHeight of tree(day1_t) : %d\n\n\n", height(day1_t, day1_t->root)); //tree의 높이 print
                printf("\n\n\n");
                printf("1. Go to menu\n");
                printf("2. exit\n");
                printf("Press number : ");
                scanf("%d", &choice);   //다음 화면 선택
                system("cls");
                if (choice == 1) {
                    continue;   //1이면 다시 초기 화면
                }
                else if (choice == 2) { //2면 프로그램 종료
                    printf("Thank you for visiting !!\n");
                    break;
                }
            }
            else if (date == 2) {   //02.12.2020 에 예약하는 경우 - 위의 과정과 동일
                inorderSeat(day2_t, day2_t->root, movie, times, seatLayout);
                printSeatLayout(seatLayout);
                printf("Please select the seat number : ");
                scanf("%d", &seat);
                printf("Will you pay by card ? (if yes enter 'y', if no enter 'n') : ");
                scanf(" %c", &payCard);
                system("cls");
                printf("Your reservation has been made successfully !!\n");
                printf("******************************************************\n");
                printResult(number[rsvNum], date, times, seat, movie, payCard);
                printf("******************************************************\n");
                rbInsert(day2_t, number[rsvNum], 2, times, movie, seat, payCard);
                printRootLeaves(day2_t);
                countNodes(day2_t, day2_t->root);
                printf("Total number of nodes : %d", cnt);
                cnt = 0;
                printf("\n\nHeight of tree(day2_t) : %d\n\n\n", height(day2_t, day2_t->root));
                printf("\n\n\n");
                printf("1. Go to menu\n");
                printf("2. exit\n");
                printf("Press number : ");
                scanf("%d", &choice);
                system("cls");
                if (choice == 1) {
                    continue;
                }
                else if (choice == 2) {
                    printf("Thank you for visiting !!\n");
                    break;
                }
            }
            else if (date == 3) {//03.12.2020 에 예약하는 경우 - 위의 과정과 동일
                inorderSeat(day3_t, day3_t->root, movie, times, seatLayout);
                printSeatLayout(seatLayout);
                printf("Please select the seat number : ");
                scanf("%d", &seat);
                printf("Will you pay by card ? (if yes enter 'y', if no enter 'n') : ");
                scanf(" %c", &payCard);
                system("cls");
                printf("Your reservation has been made successfully !!\n");
                printf("******************************************************\n");
                printResult(number[rsvNum], date, times, seat, movie, payCard);
                printf("******************************************************\n");
                rbInsert(day3_t, number[rsvNum], 3, times, movie, seat, payCard);
                printRootLeaves(day3_t);
                countNodes(day3_t, day3_t->root);
                printf("Total number of nodes : %d", cnt);
                cnt = 0;
                printf("\n\nHeight of tree(day3_t) : %d\n\n\n", height(day3_t, day3_t->root));
                printf("\n\n\n");
                printf("1. Go to menu\n");
                printf("2. exit\n");
                printf("Press number : ");
                scanf("%d", &choice);
                system("cls");
                if (choice == 1) {
                    continue;
                }
                else if (choice == 2) {
                    printf("Thank you for visiting !!\n");
                    break;
                }
            }
        }
        else if (first == 2) {  //예매 취소하는 과정
            Node* cancel;
            printf("Enter your reservation number : ");
            scanf("%d", &cancelRsvNum); //취소할 예매번호 입력
            system("cls");
            while (1) { //세 개의 트리를 돌며 해당 예매 번호의 node 찾는다
                cancel = findNode(day1_t->root, cancelRsvNum);
                if (cancel != NULL)
                    break;
                cancel = findNode(day2_t->root, cancelRsvNum);
                if (cancel != NULL)
                    break;
                cancel = findNode(day3_t->root, cancelRsvNum);
                if (cancel != NULL)
                    break;
                break;  // 못 찾으면 그냥 break 되고 cancel 노드는 NULL이 된다.
            }
            if (cancel == NULL) {   //해당 node를 못 찾았을 경우 오류 문구
                printf("You have typed the wrong reservation number. Please check once more !\n\n");
                printf("1. Go to menu\n");
                printf("2. exit\n");
                printf("Enter number : ");
                scanf("%d", &choice);
                system("cls");
                if (choice == 1) {
                    continue;
                }
                else if (choice == 2) {
                    printf("Thank you for visiting !!\n");
                    break;
                }
            }
            printf("Please check your reservation information\n\n");    //입력한 예배 번호의 예매 정보 나열
            printf("****************************\n");
            printResult(cancel->rsvNum, cancel->rsvDate, cancel->rsvTime, cancel->rsvSeat, cancel->rsvMovie, cancel->payCard);
            printf("****************************\n\n\n");
            printf("1. Cancel above reservation\n");
            printf("2. Go to menu\n");
            printf("Enter number : ");
            scanf("%d", &choice);
            system("cls");
            if (choice == 1) {  //예매 취소
                printf("Your reservation has been cancelled !\n\n\n");
                if (cancel->payCard == 'y') {
                    printf("!! Since you have selected to pay by card, refund will be made by card !!\n\n\n");
                }
                else if (cancel->payCard != 'y') {
                    printf("!! Since you have selected to pay by cash, refund will not be made !!\n\n\n");
                }
                if (cancel->rsvDate == 1) { //01.12.2020 의 예매 취소
                    rbDelete(day1_t, cancelRsvNum); //tree에서 delete
                    printRootLeaves(day1_t);    //tree의 root, leaves 노드들의 정보 나열
                    countNodes(day1_t, day1_t->root);   //tree의 총 노드수 프린트
                    printf("Total number of nodes : %d", cnt);
                    cnt = 0;
                    printf("\n\nHeight of tree(day1_t) : %d\n\n\n", height(day1_t, day1_t->root));  //트리의 높이 프린트
                    printf("\n\n\n");
                }
                else if (cancel->rsvDate == 2) {    //02.12.2020 의 예매 취소 - 위의 과정과 동일
                    rbDelete(day2_t, cancelRsvNum);
                    printRootLeaves(day2_t);
                    countNodes(day2_t, day2_t->root);
                    printf("Total number of nodes : %d", cnt);
                    cnt = 0;
                    printf("\n\nHeight of tree(day2_t) : %d\n\n\n", height(day2_t, day2_t->root));
                    printf("\n\n\n");
                }
                else if (cancel->rsvDate == 3) {    //03.12.2020 의 예매 취소 - 위의 과정과 동일
                    rbDelete(day3_t, cancelRsvNum);
                    printRootLeaves(day3_t);
                    countNodes(day3_t, day3_t->root);
                    printf("Total number of nodes : %d", cnt);
                    cnt = 0;
                    printf("\n\nHeight of tree(day3_t) : %d\n\n\n", height(day3_t, day3_t->root));
                    printf("\n\n\n");
                }
                printf("1. Go to menu\n");
                printf("2. exit\n");
                printf("Enter number : ");
                scanf("%d", &choice);
                system("cls");
                if (choice == 1) {
                    continue;
                }
                else if (choice == 2) {
                    printf("Thank you for visiting !!\n");
                    break;
                }
            }
            else if (choice == 2) { //예매 취소가 아닌 초기 화면으로 돌아간다는 번호 입력했을 경우
                continue;
            }
        }
        else if (first == 3) {  //예매도 아니고 취소도 아니고, 프로그램 종료
            printf("Thank you for visiting !\n\n\n");
            break;
        }
        else {  //1,2,3 외의 번호를 입력한 경우
            printf("Sorry, you have entered the wrong number !\n\n\n");
            continue;
        }
    }
}
int main() {
    Tree* day1_t = initTree();
    Tree* day2_t = initTree();
    Tree* day3_t = initTree();
    srand(time(NULL));
    int* A = (int*)malloc(sizeof(int) * 33); //random seat number
    int* P = (int*)malloc(sizeof(int)*33); //random pay method
    int number[2700];
    number[0] = rand() % 20000 + 10000;
    for (int i = 0; i < 2700; i++) {
        number[i] = rand() % 20000 + 10000;
        for (int j = 0; j < i; j++) {
            if (number[i] == number[j])
                i--;
        }
    }
    int rsvNum = 0;
    randomInit(A, P, day1_t, day2_t, day3_t, number, rsvNum);   //초기 30%의 좌석을 채우는 과정
    Queue* q;
    q = (Queue*)malloc(sizeof(Queue));
    initQueue(q);

    int* seatLayout;
    seatLayout = (int*)malloc(sizeof(int) * 100);   //좌석 정보
    for (int i = 0; i < 100; i++) {
        seatLayout[i] = i + 1;
    }
    UI(day1_t, day2_t, day3_t, seatLayout, number);
}
