#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <windows.h>
#include <time.h>

#define MAX_W 71
#define MAX_H 16

char maze[MAX_W][MAX_H];

typedef struct { int x, y; } Point;
typedef struct { Point data[MAX_W * MAX_H]; int front, rear; } Queue;

void initQueue(Queue *q) { q->front = 0; q->rear = 0; }
bool isEmpty(Queue *q) { return q->front == q->rear; }
void enqueue(Queue *q, Point p) { q->data[q->rear++] = p; }
Point dequeue(Queue *q) { return q->data[q->front++]; }

void hideCursor(bool hide) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = !hide;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void drawMazeOutline(int x, int y) {
    int i, j;
    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) {
            if (i == 0 && j == y - 2) maze[i][j] = '>';
            else if (i == x - 1 && j == 1) maze[i][j] = '>';
            else if (i == 0 && j == 0) maze[i][j] = (char)218;
            else if (i == x - 1 && j == 0) maze[i][j] = (char)191;
            else if (i == 0 && j == y - 1) maze[i][j] = (char)192;
            else if (i == x - 1 && j == y - 1) maze[i][j] = (char)217;
            else if (j == 0 || j == y - 1) maze[i][j] = (char)196;
            else if (i == 0 || i == x - 1) maze[i][j] = (char)179;
            else maze[i][j] = ' ';
        }
    }
}

void render(int x, int y) {
    int i, j;
    gotoxy(0, 0);
    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) { putchar(maze[i][j]); }
        putchar('\n');
    }
}

void saveMaze(int w, int h, char *fileName) {
    int i, j; char fullName[120]; FILE *file;
    sprintf(fullName, "%s.txt", fileName);
    file = fopen(fullName, "w");
    if (file == NULL) return;
    fprintf(file, "%d %d\n", w, h);
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) { fputc(maze[i][j], file); }
        fputc('\n', file);
    }
    fclose(file);
    gotoxy(0, h + 1);
    printf("\033[31m>>> \033[32m%s saved successfully!                                \033[0m", fullName);
}

void loadMaze(int *w, int *h, char *fileName) {
    int i, j; char fullName[120]; FILE *file;
    sprintf(fullName, "%s.txt", fileName);
    file = fopen(fullName, "r");
    if (file == NULL) {
        gotoxy(0, *h + 1);
        printf("\033[31m>>> \033[31mError: %s not found!                                  \033[0m", fullName);
        return;
    }
    fscanf(file, "%d %d\n", w, h);
    for (j = 0; j < *h; j++) {
        for (i = 0; i < *w; i++) { maze[i][j] = fgetc(file); }
        fgetc(file); 
    }
    fclose(file);
    system("cls"); render(*w, *h);
    gotoxy(0, *h + 1);
    printf("\033[31m>>> \033[32m%s loaded successfully!                               \033[0m", fullName);
}

void generateRandomMaze(int x, int y, int w, int h) {
    int i, dir, attempt;
    int dx[] = { 0, 0, 2, -2 }, dy[] = { 2, -2, 0, 0 }, order[] = { 0, 1, 2, 3 };
    maze[x][y] = ' ';
    for (i = 0; i < 4; i++) {
        int r = rand() % 4, temp = order[i];
        order[i] = order[r]; order[r] = temp;
    }
    for (attempt = 0; attempt < 4; attempt++) {
        dir = order[attempt];
        int nx = x + dx[dir], ny = y + dy[dir];
        if (nx > 0 && nx < w - 1 && ny > 0 && ny < h - 1 && maze[nx][ny] == (char)219) {
            maze[x + dx[dir] / 2][y + dy[dir] / 2] = ' ';
            generateRandomMaze(nx, ny, w, h);
        }
    }
}

void visualSolveBFS(int w, int h) {
    int i, j, nx, ny, stepCount;
    Point start = { 0, 0 }, end = { 0, 0 }, curr, next;
    Point parent[MAX_W][MAX_H];
    bool visited[MAX_W][MAX_H];
    Queue q;
    int dx[] = { 1, -1, 0, 0 }, dy[] = { 0, 0, 1, -1 };
    bool found = false;

    for(j=0; j<h; j++) {
        for(i=0; i<w; i++) {
            visited[i][j] = false;
            if(maze[i][j] == '.' || maze[i][j] == '*') {
                maze[i][j] = ' '; gotoxy(i,j); printf(" ");
            }
        }
    }
    for (j = 0; j < h; j++) {
        if (maze[0][j] == '>') { start.x = 0; start.y = j; }
        if (maze[w - 1][j] == '>') { end.x = w - 1; end.y = j; }
    }
    initQueue(&q); enqueue(&q, start); visited[start.x][start.y] = true;

    while (!isEmpty(&q)) {
        if (kbhit()) { if (getch() == ' ') { gotoxy(0, h+1); printf("PAUSED - Press Space to continue"); while(getch() != ' '); gotoxy(0, h+1); printf("                                "); } }
        curr = dequeue(&q);
        if (curr.x == end.x && curr.y == end.y) { found = true; break; }
        for (i = 0; i < 4; i++) {
            nx = curr.x + dx[i]; ny = curr.y + dy[i];
            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                if (!visited[nx][ny] && (maze[nx][ny] == ' ' || maze[nx][ny] == '>')) {
                    visited[nx][ny] = true; parent[nx][ny] = curr; 
                    next.x = nx; next.y = ny; enqueue(&q, next);
                    if (maze[nx][ny] == ' ') { gotoxy(nx, ny); printf("\033[33m.\033[0m"); Sleep(5); }
                }
            }
        }
    }
    if (found) {
        stepCount = 0; curr = parent[end.x][end.y];
        while (curr.x != start.x || curr.y != start.y) {
            maze[curr.x][curr.y] = '*'; stepCount++;
            gotoxy(curr.x, curr.y); printf("\033[32m*\033[0m"); Sleep(25);
            curr = parent[curr.x][curr.y];
        }
        gotoxy(0, h + 1); printf("\033[31m>>> \033[32mPath Found! Length: %d steps.              \033[0m", stepCount);
    }
}

void slowPrint(char *text, int delay) {
    int i;
    for (i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout); 
        Sleep(delay); 
    }
}

int main(void) {
    bool is_running = true;
    int mode = 3, y = 1, x = 1, width = 0, height = 0, i, j, prevX, prevY;
    char action = 0, fileName[100];
    srand(time(NULL));

    printf("Width (30-70): "); scanf("%d", &width);
    printf("Height (5-15): "); scanf("%d", &height);
    while(width > 70 || width < 30 || height > 15 || height < 5){
        system("cls");
        printf("\033[31mInvalid input !!! \033[0m\nPlease enter Width \033[31m(30-70)\033[0m and Height \033[31m(5-15)\033[0m !!!!!\n\n");
        printf("Width (30-70): "); scanf("%d", &width);
        printf("Height (5-15): "); scanf("%d", &height);
    }

    system("cls"); hideCursor(true); drawMazeOutline(width, height);
    for (i = 0; i < height; i++) { if (maze[0][i] == '>') { x = 1; y = i; break; } }
    render(width, height);
    
    gotoxy(0, height + 3);
    printf("F1:Draw | F2:Erase | F3:Move | F4:Fill | F5:Save | F6:Load \nF7:Solve | F8:Clear | F9:Random | F10:Resize \nSpace:Pause | Esc:Exit");

    while (is_running) {
        gotoxy(x, y); printf("+");
        action = getch();
        prevX = x; prevY = y;

        if (action == 0 || action == -32) {
            action = getch();
            if (action == 59) mode = 1; 
            else if (action == 60) mode = 2;
            else if (action == 61) mode = 3;
            else if (action == 62) { // F4: Fill 
                gotoxy(0, height + 3);
                printf("F1:Draw | F2:Erase | F3:Move | \033[31mF4:Fill\033[0m | F5:Save | F6:Load \nF7:Solve | F8:Clear | F9:Random | F10:Resize \nSpace:Pause | Esc:Exit");
                for (i = 1; i < height - 1; i++) {
                    for (j = 1; j < width - 1; j++) {
                        if (maze[j][i] == ' ') maze[j][i] = (char)219;
                        else if (maze[j][i] == '+') maze[j][i] = ' ';
                    }
                }
                render(width, height); 
                gotoxy(0, height + 1); printf("\033[31m>>> \033[32mMaze Filled / Updated!                 \033[0m"); 
                mode = 3; continue;
            }
            else if (action == 63) { // F5: Save
                gotoxy(0, height + 3);
                printf("F1:Draw | F2:Erase | F3:Move | F4:Fill | \033[31mF5:Save\033[0m | F6:Load \nF7:Solve | F8:Clear | F9:Random | F10:Resize \nSpace:Pause | Esc:Exit");
                hideCursor(false); gotoxy(0, height+1); printf("Save name:                      ");
                gotoxy(11, height+1); scanf("%s", fileName); hideCursor(true);
                saveMaze(width, height, fileName); continue;
            }
            else if (action == 64) { // F6: Load
                hideCursor(false); gotoxy(0, height+1); printf("File name:                      ");
                gotoxy(11, height+1); scanf("%s", fileName); hideCursor(true);
                loadMaze(&width, &height, fileName); 
                for (i = 0; i < height; i++) { if (maze[0][i] == '>') { x = 1; y = i; break; } }
                gotoxy(0, height + 3);
                printf("F1:Draw | F2:Erase | F3:Move | F4:Fill | F5:Save | \033[31mF6:Load\033[0m \nF7:Solve | F8:Clear | F9:Random | F10:Resize \nSpace:Pause | Esc:Exit");
                continue;
            }
            else if (action == 65) { // F7: Solve
            gotoxy(0, height + 3);
            printf("F1:Draw | F2:Erase | F3:Move | F4:Fill | F5:Save | F6:Load \n\033[31mF7:Solve\033[0m | F8:Clear | F9:Random | F10:Resize \nSpace:Pause | Esc:Exit"); 
            gotoxy(0, height + 1); 
            printf("\033[31m>>> \033[32mSolving Maze!                        \033[0m"); 
            visualSolveBFS(width, height); mode = 3; continue; }
            else if (action == 66) { // F8: Clear
                gotoxy(0, height + 3);
            printf("F1:Draw | F2:Erase | F3:Move | F4:Fill | F5:Save | F6:Load \nF7:Solve | \033[31mF8:Clear\033[0m | F9:Random | F10:Resize \nSpace:Pause | Esc:Exit");
                for (i = 1; i < height - 1; i++) for (j = 1; j < width - 1; j++) maze[j][i] = ' ';
                render(width, height); gotoxy(0, height + 1); printf("\033[31m>>> \033[32mMaze Cleared!                         \033[0m"); mode = 3; continue;
            }
            else if (action == 67) { // F9: Random
                gotoxy(0, height + 3);
                printf("F1:Draw | F2:Erase | F3:Move | F4:Fill | F5:Save | F6:Load \nF7:Solve | F8:Clear | \033[31mF9:Random\033[0m | F10:Resize \nSpace:Pause | Esc:Exit");
                for (i = 1; i < height - 1; i++) for (j = 1; j < width - 1; j++) maze[j][i] = (char)219;
                generateRandomMaze(1, 1, width, height);
                maze[1][height-2] = ' '; maze[width-2][1] = ' ';
                render(width, height); gotoxy(0, height + 1); printf("\033[31m>>> \033[32mRandom Maze Generated!                \033[0m"); mode = 3; continue;
            }
            else if (action == 68) { // F10: Resize
                system("cls"); hideCursor(false);
                printf("New Width (30-70): "); scanf("%d", &width);
                printf("New Height (5-15): "); scanf("%d", &height);
                if (width > 70) width = 70; if (height > 15) height = 15;
                for (i=0; i<MAX_H; i++) for (j=0; j<MAX_W; j++) maze[j][i] = ' ';
                drawMazeOutline(width, height);
                for (i=0; i<height; i++) { if (maze[0][i] == '>') { x=1; y=i; break; } }
                system("cls"); hideCursor(true); render(width, height);
                gotoxy(0, height+1); printf("\033[31m>>> \033[32mNew Size Applied!                     \033[0m"); mode = 3;
                gotoxy(0, height + 3);
                printf("F1:Draw | F2:Erase | F3:Move | F4:Fill | F5:Save | F6:Load \nF7:Solve | F8:Clear | F9:Random | \033[31mF10:Resize\033[0m \nSpace:Pause | Esc:Exit"); continue;
            }

            // Arrow keys
            if (action == 75 && x > 1) x--;
            else if (action == 77 && x < width - 2) x++;
            else if (action == 72 && y > 1) y--;
            else if (action == 80 && y < height - 2) y++;

            // Clear/Update previous position
            gotoxy(prevX, prevY);
            if (mode == 1) { 
                maze[x][y] = '+'; printf("+");
                gotoxy(0, height + 3);
                printf("\033[31mF1:Draw\033[0m | F2:Erase | F3:Move | F4:Fill | F5:Save | F6:Load \nF7:Solve | F8:Clear | F9:Random | F10:Resize \nSpace:Pause | Esc:Exit"); 
                gotoxy(0, height + 1); printf("\033[31m>>> \033[32mDrawing Mode                      \033[0m");
            } else if (mode == 2) { 
                maze[x][y] = ' '; printf(" ");
                gotoxy(0, height + 3);
                printf("F1:Draw | \033[31mF2:Erase\033[0m | F3:Move | F4:Fill | F5:Save | F6:Load \nF7:Solve | F8:Clear | F9:Random | F10:Resize \nSpace:Pause | Esc:Exit"); 
                gotoxy(0, height + 1); printf("\033[31m>>> \033[32mErasing Mode                      \033[0m");
            } else { 
                printf("%c", maze[prevX][prevY]);
                gotoxy(0, height + 3);
                printf("F1:Draw | F2:Erase | \033[31mF3:Move\033[0m | F4:Fill | F5:Save | F6:Load \nF7:Solve | F8:Clear | F9:Random | F10:Resize \nSpace:Pause | Esc:Exit"); 
                gotoxy(0, height + 1); printf("\033[31m>>> \033[32mNavigation Mode                   \033[0m");
            }
        }
        else if (action == 27) is_running = false;
    }
    system("cls");
    printf("\033[33m"); 
    slowPrint("   \"Life is a giant maze, holding a new choice at every\n", 60);
    slowPrint("    corner and a new obstacle at every wall. Getting lost\n", 60);
    slowPrint("    is not a failure; the real failure is giving up\n", 60);
    slowPrint("    looking for a way out.\n\n", 60);
    
    slowPrint("    Dead ends teach you where not to go, eventually\n", 60);
    slowPrint("    leading you to that illuminated end.\"\n\n", 60);
    
    
    printf("\033[32m"); 
    printf("\n\n    [Program shutting down. See you on new paths.]\n\n");
    printf("\033[0m");
    return 0;
}
