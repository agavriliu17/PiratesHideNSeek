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
int green[3] = {0,255,0};
int light_blue[3] = {12, 164, 255};
int blue_1[3] = {0, 105, 148};

//global variables
int screenWidth = 1400;
int screenHeight = 684;
int screen_poly[8] = {0,0, screenWidth,0, screenWidth,screenHeight, 0,screenHeight}; //lista ce determina coordonatele poligonului ecranului
bool gameOpen = true, music=true;
int mouseX, mouseY; //coordonatele mouse-ului
int pressed=0, step=1; //variabila pentru butoane din diferite meniuri

bool in_menu = true; //variabila ce verifica daca ne aflam in meniu
bool in_levels = false; //verifica daca ne aflam in pagina cu nivele
bool gameStarted = false; //variabila ce verifica daca s-a inceput jocul
bool in_settings = false; //variabila ce verifica daca suntem in setari
bool in_tutorial = false;

//variabile pentru logica matricelor
int selected_matrix[3][3] = {0,0,0,0,0,0,0,0,0};
bool is_selected = false;
bool available[4] = {1,1,1,1};
int shapes[4][3][3] =  {1,0,1,1,1,1,0,1,0,
                        1,0,1,1,1,1,1,0,1,
                        1,1,0,0,1,0,1,1,1,
                        1,1,1,0,0,1,0,0,1};



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

void draw_shape(int m[3][3], int x, int y, int lsize){
    //setcolor(RGB(light_blue[0], light_blue[1], light_blue[2]));
    int initial_x = x;
    for(int i=0;i<3;i++){
        //cout<<"Row "<<i+1<<": ("<<y<<")"<<endl;
        for(int j=0;j<3;j++){
            //cout<<"X"<<j+1<<" coordinate = "<<x<<endl;
            if(m[i][j] != 0){
                square(x,y,lsize,true,light_blue);
            }

            x+=lsize;
        }
        //cout<<"---"<<endl;
        x = initial_x;
        y += lsize;
    }
}

void shape_container(int matrix[4][3][3], int c_x, int c_y, int c_size, int gap){
    //c_x++; c_y++;
    //c_size -= 2;
    int shape_length = (c_size - gap)/2;
    int shape_size = shape_length / 3;
    draw_shape(matrix[0],c_x,c_y,shape_size);
    draw_shape(matrix[1],(c_x + gap + shape_length),c_y,shape_size);
    draw_shape(matrix[2],c_x,(c_y + gap + shape_length),shape_size);
    draw_shape(matrix[3],(c_x + gap + shape_length),(c_y + gap + shape_length),shape_size);

}

bool in_border(int mX, int mY, int x, int y, int x1, int y1){
    if (mX >= x && mX <= x1 && mY >= y && mY <= y1) return true;
    return false;
}

void copy_matrix(int m1[3][3], int m2[3][3]){//copie m2 in m1
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            m1[i][j] = m2[i][j];
        }
    }
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

void levels(){ //pagina pentru selectarea gradului de dificultate a jocului
    readimagefile("menu_background.jpg",0,0,screenWidth,screenHeight);
    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
    setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));

    //how to play

    bar(534, 257, 865, 311);
    outtextxy(700-textwidth("HOW TO PLAY")/2, 284-textheight("H")/2, "HOW TO PLAY");

    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
    //starter

    bar(535, 372, 866, 427);
    outtextxy(700-textwidth("STARTER")/2, 400-textheight("S")/2, "STARTER");


    //junior
    bar(535, 372+70, 866, 427+70);
    outtextxy(700-textwidth("JUNIOR")/2, 400+70-textheight("J")/2, "JUNIOR");

    //expert

    bar(535, 372+140, 866, 427+140);
    outtextxy(700-textwidth("EXPERT")/2, 400+140-textheight("E")/2, "EXPERT");

    //master

    bar(535, 372+210, 866, 427+210);
    outtextxy(700-textwidth("MASTER")/2, 400+210-textheight("M")/2, "MASTER");

    //back to menu

    bar(307, 609, 452, 664);
    outtextxy(379-textwidth("BACK")/2, 636-textheight("B")/2, "BACK");
}

/*void playMusic(bool music){
  if (music==true)
    PlaySound(TEXT("pirates.wav"), NULL, SND_FILENAME|SND_ASYNC);
  else
    PlaySound(NULL, 0, 0);
}*/

void settings(){ //plaseaza fundalul si textul ferestrei setari
    readimagefile("settingsbkg.jpg",0,0,screenWidth,screenHeight);
    setfillstyle(SOLID_FILL, COLOR(139, 194, 234));
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
    setbkcolor(COLOR(139, 194, 234));

    //music
    bar(452, 340, 614, 395);
    outtextxy(533-textwidth("MUSIC")/2, 367-textheight("M")/2, "MUSIC");
    bar(853, 340, 936, 395);
    outtextxy(895-textwidth("ON")/2, 367-textheight("O")/2, "ON");


    //language
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    bar(453, 442, 614, 497);
    outtextxy(534-textwidth("LANGUAGE")/2, 470-textheight("L")/2, "LANGUAGE");
    bar(853, 446, 936, 501);
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
    outtextxy(895-textwidth("EN")/2, 474-textheight("E")/2, "EN");

    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
    setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
    bar(307, 609, 452, 664);
    outtextxy(379-textwidth("BACK")/2, 636-textheight("B")/2, "BACK");
}

void howto(int step)
{
    if(step==1)
        readimagefile("howto1.jpg",0,0,screenWidth,screenHeight);
    else
        if (step==2)
        readimagefile("howto2.jpg",0,0,screenWidth,screenHeight);
            else
                readimagefile("howto3.jpg",0,0,screenWidth,screenHeight);

    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
    setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
    bar(628, 590, 773, 645);
    if(step<3)
        outtextxy(700-textwidth("NEXT")/2, 618-textheight("N")/2, "NEXT");
    else
        outtextxy(700-textwidth("OK")/2, 618-textheight("O")/2, "OK");
}

void board(){
    readimagefile("menu_background.jpg",0,0,screenWidth,screenHeight);
    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);

    //The board:
    //bar( (screenWidth/4 - 243), (screenHeight/2 - 243), (screenWidth/4 + 242), (screenHeight/2 + 242) );
    readimagefile("img2.jpg",(screenWidth/4 - 243) ,(screenHeight/2 - 243),(screenWidth/4 + 242),(screenHeight/2 + 242));

    //Sector 1 coordinates: (screenWidth/4 - 243) ,(screenHeight/2 - 243), (screenWidth/4 - 13) ,(screenHeight/2 - 13)

    //Sector 2 coordinates: (screenWidth/4 + 12) ,(screenHeight/2 - 243), (screenWidth/4 + 242) ,(screenHeight/2 - 13)

    //Sector 3 coordinates: (screenWidth/4 - 243) ,(screenHeight/2 + 12), (screenWidth/4 - 13) ,(screenHeight/2 + 242)

    //Sector 4 coordinates: (screenWidth/4 + 12) ,(screenHeight/2 + 12), (screenWidth/4 + 242) ,(screenHeight/2 + 242)


    //The shapes:
    setfillstyle(SOLID_FILL, COLOR(0, 105, 148)); //sea_blue
    bar( (3*screenWidth/4 - 243), (screenHeight/2 - 243), (3*screenWidth/4 + 242), (screenHeight/2 + 242) );
    shape_container(shapes, (3*screenWidth/4 - 243), (screenHeight/2 - 243), 485, 25);

    //Shape 1 coordinates: (3*screenWidth/4 - 243) , (screenHeight/2 - 243), (3*screenWidth/4 - 13), (screenHeight/2 - 13)

    //Shape 2 coordinates: (3*screenWidth/4 + 12) , (screenHeight/2 - 243), (3*screenWidth/4 + 242), (screenHeight/2 - 13)

    //Shape 3 coordinates: (3*screenWidth/4 - 243) , (screenHeight/2 + 12), (3*screenWidth/4 - 13), (screenHeight/2 + 242)

    //Shape 4 coordinates: (3*screenWidth/4 + 12) , (screenHeight/2 + 12), (3*screenWidth/4 + 242), (screenHeight/2 + 242)

    //Back button
    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
    setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
    bar(307, 609, 452, 664);
    outtextxy(379-textwidth("BACK")/2, 636-textheight("B")/2, "BACK");
    bar(970, 609, 1145, 664);
    outtextxy(975, 615, "RETRY");
}

void retry_level(){
    for(int i=0;i<4;i++){
        available[i] = true;
    }
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            selected_matrix[i][j] = 0;
        }
    }
    board();
}

void close_level(){
    for(int i=0;i<4;i++){
        available[i] = true;
    }
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            selected_matrix[i][j] = 0;
        }
    }
}

bool startGame(){// ciclul principal al jocului unde are loc procesarea logicii
    bool draw = true; //pentru HOW TO
    while(gameOpen){


       // primul meniu pressed=0

        getmouseclick(WM_LBUTTONDOWN,mouseX,mouseY);
        //PlaySound(TEXT("pirates.wav"), NULL, SND_FILENAME); va fi mutat intr o functie ce va fi apelata in setari
       // Folosim variabilele 'in_menu', 'in_levels', 'gameStarted' in loc de variabila pressed pentru lucru mai usor cu
       // programul si o structura mai simpla

        if(in_menu){//atat timp cat suntem in meniu
            if(in_border(mouseX, mouseY, 575, 320, 829, 392)){ //intram pe pagina cu nivele
                clearmouseclick(WM_LBUTTONDOWN);
                in_levels = true; in_menu = false;
                levels();
            }
            if(in_border(mouseX, mouseY, 575, 320+120, 829, 392+120)){ //intram in setari
                clearmouseclick(WM_LBUTTONDOWN);
                in_settings = true; in_menu = false;
                settings();
            }
            if(in_border(mouseX, mouseY, 575,560, 866,632)){ //inchidem jocul
                clearmouseclick(WM_LBUTTONDOWN);
                gameOpen = false;
            }
        }

        if(in_levels){//atat timp cat suntem pe pagina 'nivele'
            if(in_border(mouseX, mouseY, 307,609, 452,664)){ //revenim in meniu
                clearmouseclick(WM_LBUTTONDOWN);
                in_levels = false; in_menu = true;
                menu();
            }
            if(in_border(mouseX,mouseY,535, 372, 866, 427)){//nivelul starter
                clearmouseclick(WM_LBUTTONDOWN);
                gameStarted = true; in_levels = false;
                board();
            }
            if(in_border(mouseX,mouseY,534, 257, 865, 311)){
                clearmouseclick(WM_LBUTTONDOWN);
                in_levels = false; in_tutorial = true;
                step = 1;
            }

        }
        if(gameStarted){


                        //selectarea figurilor
            if(in_border(mouseX,mouseY, (3*screenWidth/4 - 243) , (screenHeight/2 - 243), (3*screenWidth/4 - 13), (screenHeight/2 - 13)) && available[0]){
                copy_matrix(selected_matrix , shapes[0]);
                square((3*screenWidth/4 - 243) , (screenHeight/2 - 243),230,true,blue_1); is_selected = true;
                available[0] = 0;
            }
            if(in_border(mouseX,mouseY, (3*screenWidth/4 + 12) , (screenHeight/2 - 243), (3*screenWidth/4 + 242), (screenHeight/2 - 13)) && available[1]){
                copy_matrix(selected_matrix , shapes[1]);
                square((3*screenWidth/4 + 12) , (screenHeight/2 - 243),230,true,blue_1); is_selected = true;
                available[1] = 0;
            }
            if(in_border(mouseX,mouseY, (3*screenWidth/4 - 243) , (screenHeight/2 + 12), (3*screenWidth/4 - 13), (screenHeight/2 + 242)) && available[2]){
                copy_matrix(selected_matrix , shapes[2]);
                square((3*screenWidth/4 - 243) , (screenHeight/2 + 12),230,true,blue_1); is_selected = true;
                available[2] = 0;
            }
            if(in_border(mouseX,mouseY, (3*screenWidth/4 + 12) , (screenHeight/2 + 12), (3*screenWidth/4 + 242), (screenHeight/2 + 242)) && available[3]){
                copy_matrix(selected_matrix , shapes[3]);
                square((3*screenWidth/4 + 12) , (screenHeight/2 + 12),230,true,blue_1); is_selected = true;
                available[3] = 0;
            }


                        //plasarea figurilor
            if (is_selected && in_border(mouseX,mouseY, (screenWidth/4 - 243) ,(screenHeight/2 - 243), (screenWidth/4 - 13) ,(screenHeight/2 - 13))){
                draw_shape(selected_matrix,(screenWidth/4 - 243) ,(screenHeight/2 - 243),230/3); is_selected = false;
            }

            if (is_selected && in_border(mouseX,mouseY, (screenWidth/4 + 12) ,(screenHeight/2 - 243), (screenWidth/4 + 242) ,(screenHeight/2 - 13))){
                draw_shape(selected_matrix,(screenWidth/4 + 12) ,(screenHeight/2 - 243),230/3); is_selected = false;
            }

            if (is_selected && in_border(mouseX,mouseY, (screenWidth/4 - 243) ,(screenHeight/2 + 12), (screenWidth/4 - 13) ,(screenHeight/2 + 242))){
                draw_shape(selected_matrix,(screenWidth/4 - 243) ,(screenHeight/2 + 12),230/3); is_selected = false;
            }

            if (is_selected && in_border(mouseX,mouseY, (screenWidth/4 + 12) ,(screenHeight/2 + 12), (screenWidth/4 + 242) ,(screenHeight/2 + 242))){
                draw_shape(selected_matrix,(screenWidth/4 + 12) ,(screenHeight/2 + 12),230/3); is_selected = false;
            }


                        //Retry

            if(in_border(mouseX, mouseY, 970, 609, 1145, 664)){
                clearmouseclick(WM_LBUTTONDOWN);
                retry_level();
            }

                        //Back
            if (in_border(mouseX, mouseY, 307, 609, 452, 664)){
                in_levels = true; gameStarted = false;
                close_level();
                levels();
            }
        }

        if(in_settings){
            if(mouseX > 853 && mouseX < 936 && mouseY > 340 && mouseY < 395){ //music on/off
                clearmouseclick(WM_LBUTTONDOWN);
                if (music==true)
                {
                    music=false;
                    //playMusic(music);
                    setfillstyle(SOLID_FILL, COLOR(139, 194, 234));
                    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
                    setbkcolor(COLOR(139, 194, 234));
                    bar(853, 340, 936, 395);
                    outtextxy(895-textwidth("OFF")/2, 367-textheight("O")/2, "OFF");
                }
                else
                {
                    music=true;
                    //playMusic(music);
                    setfillstyle(SOLID_FILL, COLOR(139, 194, 234));
                    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
                    setbkcolor(COLOR(139, 194, 234));
                    bar(853, 340, 936, 395);
                    outtextxy(895-textwidth("ON")/2, 367-textheight("O")/2, "ON");
                }
            }
            if(in_border(mouseX,mouseY,307, 609, 452, 664)){
                clearmouseclick(WM_LBUTTONDOWN);
                in_settings = false; in_menu = true;
                menu();
            }
        }

        if(in_tutorial){

            if(draw){
                draw = false;
                howto(step);
            }

            if(in_border(mouseX, mouseY, 628, 590, 773, 645)){
                clearmouseclick(WM_LBUTTONDOWN);
                if(step < 3){
                    step ++;
                    draw = true;
                }
                else{
                    in_tutorial = false; in_levels = true;
                    levels();
                }
            }

        }


    }
    return gameOpen;
}

int main(){

    initwindow(screenWidth,screenHeight);
    menu();
    //playMusic(music);
    startGame();

    closegraph();
    return 0;
}
