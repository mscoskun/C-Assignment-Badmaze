#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <windows.h>
#include <time.h>

#define MAX_W 71
#define MAX_H 16

char labirent[MAX_W][MAX_H];

typedef struct { int x, y; } Point;
typedef struct { Point data[MAX_W * MAX_H]; int front, rear; } Queue;

void initQueue(Queue *q) { q->front = 0; q->rear = 0; }
bool isEmpty(Queue *q) { return q->front == q->rear; }
void enqueue(Queue *q, Point p) { q->data[q->rear++] = p; }
Point dequeue(Queue *q) { return q->data[q->front++]; }

void imleciGizle(bool gizle) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = !gizle;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void labirentciz(int x, int y) {
    int i, j;
    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) {
            if (i == 0 && j == y - 2) labirent[i][j] = '>';
            else if (i == x - 1 && j == 1) labirent[i][j] = '>';
            else if (i == 0 && j == 0) labirent[i][j] = (char)218;
            else if (i == x - 1 && j == 0) labirent[i][j] = (char)191;
            else if (i == 0 && j == y - 1) labirent[i][j] = (char)192;
            else if (i == x - 1 && j == y - 1) labirent[i][j] = (char)217;
            else if (j == 0 || j == y - 1) labirent[i][j] = (char)196;
            else if (i == 0 || i == x - 1) labirent[i][j] = (char)179;
            else labirent[i][j] = ' ';
        }
    }
}

void ciz(int x, int y) {
    int i, j;
    gotoxy(0, 0);
    for (j = 0; j < y; j++) {
        for (i = 0; i < x; i++) { putchar(labirent[i][j]); }
        putchar('\n');
    }
}

void labirentKaydet(int w, int h, char *dosyaAdi) {
    int i, j; char tamIsim[120]; FILE *dosya;
    sprintf(tamIsim, "%s.txt", dosyaAdi);
    dosya = fopen(tamIsim, "w");
    if (dosya == NULL) return;
    fprintf(dosya, "%d %d\n", w, h);
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) { fputc(labirent[i][j], dosya); }
        fputc('\n', dosya);
    }
    fclose(dosya);
    gotoxy(0, h + 1);
    printf("\033[31m>>> \033[32m%s kaydedildi!                                \033[0m", tamIsim);
}

void labirentYukle(int *w, int *h, char *dosyaAdi) {
    int i, j; char tamIsim[120]; FILE *dosya;
    sprintf(tamIsim, "%s.txt", dosyaAdi);
    dosya = fopen(tamIsim, "r");
    if (dosya == NULL) {
        gotoxy(0, *h + 1);
        printf("\033[31m>>> \033[31mHata: %s bulunamadi!                          \033[0m", tamIsim);
        return;
    }
    fscanf(dosya, "%d %d\n", w, h);
    for (j = 0; j < *h; j++) {
        for (i = 0; i < *w; i++) { labirent[i][j] = fgetc(dosya); }
        fgetc(dosya); 
    }
    fclose(dosya);
    system("cls"); ciz(*w, *h);
    gotoxy(0, *h + 1);
    printf("\033[31m>>> \033[32m%s yuklendi!                                  \033[0m", tamIsim);
}

void rastgeleLabirentOlustur(int x, int y, int w, int h) {
    int i, yon, deneme;
    int dx[] = { 0, 0, 2, -2 }, dy[] = { 2, -2, 0, 0 }, sira[] = { 0, 1, 2, 3 };
    labirent[x][y] = ' ';
    for (i = 0; i < 4; i++) {
        int r = rand() % 4, temp = sira[i];
        sira[i] = sira[r]; sira[r] = temp;
    }
    for (deneme = 0; deneme < 4; deneme++) {
        yon = sira[deneme];
        int nx = x + dx[yon], ny = y + dy[yon];
        if (nx > 0 && nx < w - 1 && ny > 0 && ny < h - 1 && labirent[nx][ny] == (char)219) {
            labirent[x + dx[yon] / 2][y + dy[yon] / 2] = ' ';
            rastgeleLabirentOlustur(nx, ny, w, h);
        }
    }
}

void cozumGorselBFS(int w, int h) {
    int i, j, nx, ny, adim;
    Point start = { 0, 0 }, end = { 0, 0 }, curr, next;
    Point parent[MAX_W][MAX_H];
    bool visited[MAX_W][MAX_H];
    Queue q;
    int dx[] = { 1, -1, 0, 0 }, dy[] = { 0, 0, 1, -1 };
    bool found = false;

    for(j=0; j<h; j++) {
        for(i=0; i<w; i++) {
            visited[i][j] = false;
            if(labirent[i][j] == '.' || labirent[i][j] == '*') {
                labirent[i][j] = ' '; gotoxy(i,j); printf(" ");
            }
        }
    }
    for (j = 0; j < h; j++) {
        if (labirent[0][j] == '>') { start.x = 0; start.y = j; }
        if (labirent[w - 1][j] == '>') { end.x = w - 1; end.y = j; }
    }
    initQueue(&q); enqueue(&q, start); visited[start.x][start.y] = true;

    while (!isEmpty(&q)) {
        if (kbhit()) { if (getch() == ' ') { gotoxy(0, h+1); printf("DURAKLATILDI - Devam icin Space"); while(getch() != ' '); gotoxy(0, h+1); printf("                                "); } }
        curr = dequeue(&q);
        if (curr.x == end.x && curr.y == end.y) { found = true; break; }
        for (i = 0; i < 4; i++) {
            nx = curr.x + dx[i]; ny = curr.y + dy[i];
            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                if (!visited[nx][ny] && (labirent[nx][ny] == ' ' || labirent[nx][ny] == '>')) {
                    visited[nx][ny] = true; parent[nx][ny] = curr; 
                    next.x = nx; next.y = ny; enqueue(&q, next);
                    if (labirent[nx][ny] == ' ') { gotoxy(nx, ny); printf("\033[33m.\033[0m"); Sleep(5); }
                }
            }
        }
    }
    if (found) {
        adim = 0; curr = parent[end.x][end.y];
        while (curr.x != start.x || curr.y != start.y) {
            labirent[curr.x][curr.y] = '*'; adim++;
            gotoxy(curr.x, curr.y); printf("\033[32m*\033[0m"); Sleep(25);
            curr = parent[curr.x][curr.y];
        }
        gotoxy(0, h + 1); printf("\033[31m>>> \033[32mYol Bulundu! Uzunluk: %d adim.             \033[0m", adim);
    }
}

void yavasYaz(char *metin, int gecikme) {
    int i;
    for (i = 0; metin[i] != '\0'; i++) {
        printf("%c", metin[i]);
        fflush(stdout); // Karakterin anýnda ekrana basýlmasýný saðlar
        Sleep(gecikme); // Her karakter arasý bekleme süresi (milisaniye)
    }
}

int main(void) {
    bool secim = true;
    int mod = 3, y = 1, x = 1, z = 0, k = 0, i, j, eskiX, eskiY;
    char islem = 0, dosyaIsmi[100];
    srand(time(NULL));

    printf("Genislik (30-70): "); scanf("%d", &z);
    printf("Yukseklik (5-15): "); scanf("%d", &k);
    	while(z>70 || z<30 || k>15 || k<5){
    		system("cls");
    		printf("\033[31mYanlis giris yaptiniz !!! \033[0m\nLutfen Genislik \033[31m(30-70)\033[0m ve Yukseklik \033[31m(5-15)\033[0m olacak sekilde giris yapiniz !!!!!\n\n");
    		printf("Genislik (30-70): "); scanf("%d", &z);
    		printf("Yukseklik (5-15): "); scanf("%d", &k);
		}

    system("cls"); imleciGizle(true); labirentciz(z, k);
    for (i = 0; i < k; i++) { if (labirent[0][i] == '>') { x = 1; y = i; break; } }
    ciz(z, k);
    
    gotoxy(0, k + 3);
    printf("F1:Ciz | F2:Sil | F3:Gez | F4:Doldur | F5:Kaydet | F6:Yukle \nF7:Coz | F8:Temizle | F9:Rastgele | F10:Yeni Boyut \nSpace:Pause | Esc:Cikis");

    while (secim) {
        gotoxy(x, y); printf("+");
        islem = getch();
        eskiX = x; eskiY = y;

        if (islem == 0 || islem == -32) {
            islem = getch();
            if (islem == 59) mod = 1; 
            else if (islem == 60) mod = 2;
            else if (islem == 61) mod = 3;
            else if (islem == 62) { // F4: Doldur 
            	gotoxy(0, k + 3);
        		printf("F1:Ciz | F2:Sil | F3:Gez | \033[31mF4:Doldur\033[0m | F5:Kaydet | F6:Yukle \nF7:Coz | F8:Temizle | F9:Rastgele | F10:Yeni Boyut \nSpace:Pause | Esc:Cikis");
                for (i = 1; i < k - 1; i++) {
                    for (j = 1; j < z - 1; j++) {
                        if (labirent[j][i] == ' ') labirent[j][i] = (char)219;
                        else if (labirent[j][i] == '+') labirent[j][i] = ' ';
                    }
                }
                ciz(z, k); 
                gotoxy(0, k + 1); printf("\033[31m>>> \033[32mLabirent Dolduruldu / Guncellendi!     \033[0m"); 
                mod=3; continue;
            }
            else if (islem == 63) { // F5: Kaydet
            	gotoxy(0, k + 3);
        		printf("F1:Ciz | F2:Sil | F3:Gez | F4:Doldur | \033[31mF5:Kaydet\033[0m | F6:Yukle \nF7:Coz | F8:Temizle | F9:Rastgele | F10:Yeni Boyut \nSpace:Pause | Esc:Cikis");
                imleciGizle(false); gotoxy(0, k+1); printf("Kayit ismi:                     ");
                gotoxy(12, k+1); scanf("%s", dosyaIsmi); imleciGizle(true);
                labirentKaydet(z, k, dosyaIsmi); continue;
            }
            else if (islem == 64) { // F6: Yukle
                imleciGizle(false); gotoxy(0, k+1); printf("Dosya ismi:                     ");
                gotoxy(12, k+1); scanf("%s", dosyaIsmi); imleciGizle(true);
                labirentYukle(&z, &k, dosyaIsmi); 
                for (i = 0; i < k; i++) { if (labirent[0][i] == '>') { x = 1; y = i; break; } }
                gotoxy(0, k + 3);
        		printf("F1:Ciz | F2:Sil | F3:Gez | F4:Doldur | F5:Kaydet | \033[31mF6:Yukle\033[0m \nF7:Coz | F8:Temizle | F9:Rastgele | F10:Yeni Boyut \nSpace:Pause | Esc:Cikis");
                continue;
            }
            else if (islem == 65) { // F7: Labirent cozme
			gotoxy(0, k + 3);
        	printf("F1:Ciz | F2:Sil | F3:Gez | F4:Doldur | F5:Kaydet | F6:Yukle \n\033[31mF7:Coz\033[0m | F8:Temizle | F9:Rastgele | F10:Yeni Boyut \nSpace:Pause | Esc:Cikis"); 
			gotoxy(0, k + 1); 
			printf("\033[31m>>> \033[32mLabirent Cozuluyor!                  \033[0m"); 
			cozumGorselBFS(z, k); mod=3;continue; }
            else if (islem == 66) { // F8: Temizle
            	gotoxy(0, k + 3);
        	printf("F1:Ciz | F2:Sil | F3:Gez | F4:Doldur | F5:Kaydet | F6:Yukle \nF7:Coz | \033[31mF8:Temizle\033[0m | F9:Rastgele | F10:Yeni Boyut \nSpace:Pause | Esc:Cikis");
                for (i = 1; i < k - 1; i++) for (j = 1; j < z - 1; j++) labirent[j][i] = ' ';
                ciz(z, k); gotoxy(0, k + 1); printf("\033[31m>>> \033[32mLabirent Temizlendi!                  \033[0m"); mod=3; continue;
            }
            else if (islem == 67) { // F9: Rastgele
            	gotoxy(0, k + 3);
				printf("F1:Ciz | F2:Sil | F3:Gez | F4:Doldur | F5:Kaydet | F6:Yukle \nF7:Coz | F8:Temizle | \033[31mF9:Rastgele\033[0m | F10:Yeni Boyut \nSpace:Pause | Esc:Cikis");
                for (i = 1; i < k - 1; i++) for (j = 1; j < z - 1; j++) labirent[j][i] = (char)219;
                rastgeleLabirentOlustur(1, 1, z, k);
                labirent[1][k-2] = ' '; labirent[z-2][1] = ' ';
                ciz(z, k); gotoxy(0, k + 1); printf("\033[31m>>> \033[32mRastgele Labirent Olusturuldu!        \033[0m"); mod=3; continue;
            }
            else if (islem == 68) { // F10: Yeni Boyut
                system("cls"); imleciGizle(false);
                printf("Yeni Genislik (30-70): "); scanf("%d", &z);
                printf("Yeni Yukseklik (5-15): "); scanf("%d", &k);
                if (z > 70) z = 70; if (k > 15) k = 15;
                for (i=0; i<MAX_H; i++) for (j=0; j<MAX_W; j++) labirent[j][i] = ' ';
                labirentciz(z, k);
                for (i=0; i<k; i++) { if (labirent[0][i] == '>') { x=1; y=i; break; } }
                system("cls"); imleciGizle(true); ciz(z, k);
                gotoxy(0, k+1); printf("\033[31m>>> \033[32mYeni Boyut Ayarlandi!                 \033[0m"); mod=3;
				gotoxy(0, k + 3);
        		printf("F1:Ciz | F2:Sil | F3:Gez | F4:Doldur | F5:Kaydet | F6:Yukle \nF7:Coz | F8:Temizle | F9:Rastgele | \033[31mF10:Yeni Boyut\033[0m \nSpace:Pause | Esc:Cikis"); continue;
            }

            // Yön tuþlarý
            if (islem == 75 && x > 1) x--;
            else if (islem == 77 && x < z - 2) x++;
            else if (islem == 72 && y > 1) y--;
            else if (islem == 80 && y < k - 2) y++;

            // Eski konumu temizle/güncelle
            gotoxy(eskiX, eskiY);
            if (mod == 1) { 
                labirent[x][y] = '+'; printf("+");
				gotoxy(0, k + 3);
        		printf("\033[31mF1:Ciz\033[0m | F2:Sil | F3:Gez | F4:Doldur | F5:Kaydet | F6:Yukle \nF7:Coz | F8:Temizle | F9:Rastgele | F10:Yeni Boyut \nSpace:Pause | Esc:Cikis"); 
                gotoxy(0, k + 1); printf("\033[31m>>> \033[32mCizim modu                        \033[0m");
            } else if (mod == 2) { 
                labirent[x][y] = ' '; printf(" ");
				gotoxy(0, k + 3);
        		printf("F1:Ciz | \033[31mF2:Sil\033[0m | F3:Gez | F4:Doldur | F5:Kaydet | F6:Yukle \nF7:Coz | F8:Temizle | F9:Rastgele | F10:Yeni Boyut \nSpace:Pause | Esc:Cikis"); 
                gotoxy(0, k + 1); printf("\033[31m>>> \033[32mSilme modu                        \033[0m");
            } else { 
                printf("%c", labirent[eskiX][eskiY]);
				gotoxy(0, k + 3);
        		printf("F1:Ciz | F2:Sil | \033[31mF3:Gez\033[0m | F4:Doldur | F5:Kaydet | F6:Yukle \nF7:Coz | F8:Temizle | F9:Rastgele | F10:Yeni Boyut \nSpace:Pause | Esc:Cikis"); 
                gotoxy(0, k + 1); printf("\033[31m>>> \033[32mGezinti modu                      \033[0m");
            }
        }
        else if (islem == 27) secim = false;
    }
    system("cls");
	printf("\033[33m"); 
    yavasYaz("   \"Hayat, her kosesinde yeni bir tercih, her duvarinda yeni bir\n", 60);
    yavasYaz("    engel barindiran devasa bir labirenttir. Kaybolmak bir\n", 60);
    yavasYaz("    basarisizlik degil; asil basarisizlik, bir cikis yolu \n", 60);
    yavasYaz("    aramaktan vazgecmektir.\n\n", 60);
    
    yavasYaz("    Cikmaz sokaklar sana nereye gitmemen gerektigini ogretir,\n", 60);
    yavasYaz("    boylece eninde sonunda seni o isikli sona ulastirir.\"\n\n", 60);
    
    
    printf("\033[32m"); 
    printf("\n\n    [Program kapaniyor. Yeni yollarda gorusmek uzere.]\n\n");
    printf("\033[0m");
    return 0;
}
