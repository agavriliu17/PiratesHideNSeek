#include <iostream>
#include <graphics.h>
#include <winbgim.h>
#include <windows.h>
#include <mmsystem.h>
using namespace std;
#define N 3

// rgb colors
int black[3] = {0,0,0};
int white[3] = {255,255,255};
int dark_blue[3] = {29, 85, 173};
int sea_blue[3] = {152, 65, 21};
//global variables
int screenWidth = 1400;
int screenHeight = 684;
int screen_poly[8] = {0,0, screenWidth,0, screenWidth,screenHeight, 0,screenHeight}; //lista ce determina coordonatele poligonului ecranului
bool gameOpen = true;
int mouseX, mouseY; //coordonatele mouse-ului

void hide_matrix(int a[][3],int b[][3]){//logica pentru suprapunerea matricelor
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(b[i][j]==1){
                a[i][j] = 0;
            }
        }
    }
}

void square(int x, int y, int size, bool fillshape,int color[3]){ //deseneaza un patrat cu coordonatele coltului stang sus (x,y), latura de marime size si culoarea rgb color,
                                                                  //daca fillshape e true se umple interiorul patratului cu culoarea selectata
    int shape[8] = {x,y, (x+size),y, (x+size),(y+size), x,(y+size)};
    if (fillshape){
        setfillstyle(SOLID_FILL,COLOR(color[0], color[1], color[2]));
        fillpoly(4,shape);
    }
    else{
        line(x,y, (x+size),y);
        line((x+size),y, (x+size),(y+size));
        line((x+size),(y+size), x,(y+size));
        line(x,(y+size), x,y);
    }
}

void fill_rect(int x, int y, int size_x, int size_y, int color[3]){ //umple un dreptunghi cu laturile size_x size_y si coordonatele (x,y), coloarea rgb - color
    int shape[8] = {x,y, (x+size_x),y, (x+size_x),(y+size_y), x,(y+size_y)};
    setfillstyle(SOLID_FILL,COLOR(color[0], color[1], color[2]));
    fillpoly(4,shape);

}

void menu(){ //plaseaza fundalul si textul ferestrei de meniu
    readimagefile("menu_background.jpg",0,0,screenWidth,screenHeight);
    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
    settextstyle(BOLD_FONT, HORIZ_DIR, 6);
    setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));

    //start

    bar(575, 320, 829, 392);
    outtextxy(702-textwidth("START")/2, 356-textheight("S")/2, "START");


    //settings
    bar(575, 320+120, 829, 392+120);
    outtextxy(702-textwidth("SETTINGS")/2, 476-textheight("S")/2, "SETTINGS");

    //exit

    bar(575, 320+240, 829, 392+240);
    outtextxy(702-textwidth("EXIT")/2, 596-textheight("E")/2, "EXIT");

}

int settings(){ //plaseaza fundalul si textul ferestrei setari
    bool settingsOpen = true;
    while(settingsOpen){
        ;
    }
    return 1;
}
bool startGame(){ // ciclul principal al jocului unde are loc procesarea logicii
    while(gameOpen){
        PlaySound(TEXT("pirates.wav"), NULL, SND_FILENAME); //va fi mutat intr o functie ce va fi apelata in setari
        getmouseclick(WM_LBUTTONDOWN,mouseX,mouseY);
        if(mouseX > (screenWidth/2 - 200) && mouseX < (screenWidth/2 +200) && mouseY > (screenHeight/4 + 360) && mouseY < (screenHeight/4 + 440)){
            clearmouseclick(WM_LBUTTONDOWN);
            gameOpen = false;
        }

    }
    return gameOpen;
}

int main(){

    initwindow(screenWidth,screenHeight);
    menu();
    startGame();

    closegraph();
    return 0;
}
