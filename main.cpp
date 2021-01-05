#include <iostream>
#include <graphics.h>
#include <winbgim.h>
#include <windows.h>
#include <mmsystem.h>
#include <fstream>
#include <ctime>
using namespace std;
#define N 3

// rgb colors
int black[3] = {0,0,0};
int white[3] = {255,255,255};
int dark_blue[3] = {29, 85, 173};
int sea_blue[3] = {152, 65, 21};
int green[3] = {0,255,0};
int light_blue[3] = {14, 197, 219};
int blue_1[3] = {0, 105, 148};
int blue_2[3]={71, 179, 191};

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
bool playingStarter = false, playingJunior = false, playingExpert = false, playingMaster = false, playingGenerated = false; //variabile ce verifica nivelul selectat
bool in_english = true;
bool win_screen = false;
bool lose_screen = false;
bool hint_available = true;


//variabile pentru logica matricelor
int placed=0;
int selected_matrix[3][3] = {0,0,0,0,0,0,0,0,0};
bool is_selected = false;
bool available[4] = {1,1,1,1};
bool placed_pieces[4] = {0,0,0,0};
int shapes[4][3][3];
struct coordonatePiese {
    int x, y;
}matrix_coord[4];


int challenge[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int board_matrix[4][3][3];
int solution[4];
int solution_rotation[4];

void rotate90Clockwise(int a[N][N]) //functie pentru a roti piesa selectata
{

    for (int i = 0; i < N / 2; i++) {
        for (int j = i; j < N - i - 1; j++) {
            int aux = a[i][j];
            a[i][j] = a[N - 1 - j][i];
            a[N - 1 - j][i] = a[N - 1 - i][N - 1 - j];
            a[N - 1 - i][N - 1 - j] = a[j][N - 1 - i];
            a[j][N - 1 - i] = aux;
        }
    }
}

void refresh_board(int squares, int first_poz, int selected_level){ //citeste din nou matricea tablei de joc: squares - cate patrate trebuie citite,
                                                //                                        first_poz - de la care patrat incepem citirea
    ifstream fi; string aux;
    if (selected_level == 1){
        fi.open("starter/board.in", ios::in);
    }
    else if (selected_level == 2){
        fi.open("junior/board.in", ios::in);
    }
    else if (selected_level == 3){
        fi.open("expert/board.in", ios::in);
    }
    else if (selected_level == 4){
        fi.open("master/board.in", ios::in);
    }
    else if (selected_level == 5){
        fi.open("generated/board.in", ios::in);
    }
    else{
        cout<<"Error: No suck level!"<<endl;
        return;
    }

    int k = 0;
    if(first_poz+squares > 4){
        cout<<"Error, square out of range!"<<endl;
        return;
    }
    while(k < first_poz){
        getline(fi,aux);
        k++;
    }
    k = 0;
    while(k < squares){
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                fi>>board_matrix[first_poz+k][i][j];
            }
        }
        k++;
    }
    fi.close();
}

void read_shapes(){
    ifstream fi;
    string aux;
    fi.open("generated/shapes.in", ios::in);
    srand(time(0));
    int na[4] = {-1,-1,-1,-1}, rng, k = 0;
    while(k < 4){
        rng = rand() % 13;
        if(rng != na[0] && rng != na[1] && rng != na[2] && rng != na[3]){
            //cout<<"The line is: "<<rng+1<<endl;
            for(int f = 0; f < rng; f++){
                getline(fi, aux);
            }
            //getline(fi,aux);
            //cout<<aux<<endl;
            for(int i = 0; i < 3; i++){
                for(int j = 0; j < 3; j++){
                    fi>>shapes[k][i][j];

                }
            }
            na[k] = rng;
            k++;
            fi.clear();
            fi.seekg(0, ios::beg);
        }
    }
    fi.close();
}
//End

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

void draw_shape(int m[3][3], int x, int y, int lsize, int color[3]){
    //setcolor(RGB(light_blue[0], light_blue[1], light_blue[2]));
    int initial_x = x;
    for(int i=0;i<3;i++){
        //cout<<"Row "<<i+1<<": ("<<y<<")"<<endl;
        for(int j=0;j<3;j++){
            //cout<<"X"<<j+1<<" coordinate = "<<x<<endl;
            if(m[i][j] != 0){
                square(x,y,lsize,true,color);
            }

            x+=lsize;
        }
        //cout<<"---"<<endl;
        x = initial_x;
        y += lsize;
    }
}

void shape_container(int matrix[4][3][3], int c_x, int c_y, int c_size, int color1[3], int color2[3], int nrPiesa){
    //c_x++; c_y++;
    //c_size -= 2;
    square( c_x, c_y, c_size/2-1, true, color1);
    int shape_length = (c_size)/2;
    int shape_size = shape_length / 3;
    draw_shape(matrix[nrPiesa],c_x,c_y,shape_size, color2);
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
    if(in_english){
        outtextxy(702-textwidth("SETTINGS")/2, 476-textheight("S")/2, "SETTINGS");
    }
    else{
        outtextxy(702-textwidth("SETARI")/2, 476-textheight("S")/2, "SETARI");
    }


    //exit

    bar(575, 320+240, 829, 392+240);
    if(in_english){
        outtextxy(702-textwidth("EXIT")/2, 596-textheight("E")/2, "EXIT");
    }
    else{
        outtextxy(702-textwidth("IESIRE")/2, 596-textheight("I")/2, "IESIRE");
    }


}

void levels(){ //pagina pentru selectarea gradului de dificultate a jocului
    readimagefile("menu_background.jpg",0,0,screenWidth,screenHeight);
    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
    setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));


    //how to play

    bar(534, 257, 865, 311);
    if(in_english){
        outtextxy(700-textwidth("HOW TO PLAY")/2, 284-textheight("H")/2, "HOW TO PLAY");
    }
    else{
        outtextxy(700-textwidth("CUM JOCAM")/2, 284-textheight("C")/2, "CUM JUCAM");
    }


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
    if(in_english){
        outtextxy(379-textwidth("BACK")/2, 636-textheight("B")/2, "BACK");
    }
    else{
        outtextxy(379-textwidth("INAPOI")/2, 636-textheight("I")/2, "INAPOI");
    }
    //generated level
    settextstyle(BOLD_FONT, HORIZ_DIR, 3);
    bar(949, 509, 1140, 564);
    if(in_english){
        outtextxy(1044-textwidth("GENERATED")/2, 536-textheight("G")/2, "GENERATED");
    }
    else{
        outtextxy(1044-textwidth("GENERAT")/2, 536-textheight("G")/2, "GENERAT");
    }
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
    if(in_english){
        outtextxy(533-textwidth("MUSIC")/2, 367-textheight("M")/2, "MUSIC");
    }
    else{
        outtextxy(533-textwidth("MUZICA")/2, 367-textheight("M")/2, "MUZICA");
    }
    bar(853, 340, 936, 395);
    if(in_english){
        outtextxy(895-textwidth("ON")/2, 367-textheight("O")/2, "ON");
    }
    else{
        outtextxy(895-textwidth("DA")/2, 367-textheight("D")/2, "DA");
    }



    //language
    settextstyle(BOLD_FONT, HORIZ_DIR, 4);
    bar(453, 442, 614, 497);
    if(in_english){
        outtextxy(534-textwidth("LANGUAGE")/2, 470-textheight("L")/2, "LANGUAGE");
    }
    else{
        outtextxy(534-textwidth("LIMBA")/2, 470-textheight("L")/2, "LIMBA");
    }

    bar(853, 446, 936, 501);
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
    if(in_english){
        outtextxy(895-textwidth("EN")/2, 474-textheight("E")/2, "EN");
    }
    else{
        outtextxy(895-textwidth("RO")/2, 474-textheight("R")/2, "RO");
    }

    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
    setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
    bar(307, 609, 452, 664);
    if(in_english){
        outtextxy(379-textwidth("BACK")/2, 636-textheight("B")/2, "BACK");
    }
    else{
        outtextxy(379-textwidth("INAPOI")/2, 636-textheight("I")/2, "INAPOI");
    }

}

void howto(int step)
{
    if(in_english){
       if(step==1)
            readimagefile("english/howto1.jpg",0,0,screenWidth,screenHeight);
        else
            if (step==2)
            readimagefile("english/howto2.jpg",0,0,screenWidth,screenHeight);
                else
                    readimagefile("english/howto3.jpg",0,0,screenWidth,screenHeight);

        setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
        settextstyle(BOLD_FONT, HORIZ_DIR, 5);
        setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
        bar(628, 590, 773, 645);
        if(step<3)
            outtextxy(700-textwidth("NEXT")/2, 618-textheight("N")/2, "NEXT");
        else
            outtextxy(700-textwidth("OK")/2, 618-textheight("O")/2, "OK");
    }
    else{
       if(step==1)
            readimagefile("romanian/howto1.jpg",0,0,screenWidth,screenHeight);
        else
            if (step==2)
            readimagefile("romanian/howto2.jpg",0,0,screenWidth,screenHeight);
                else
                    readimagefile("romanian/howto3.jpg",0,0,screenWidth,screenHeight);

        setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
        settextstyle(BOLD_FONT, HORIZ_DIR, 5);
        setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
        bar(628, 590, 773, 645);
        if(step<3)
            outtextxy(700-textwidth("URMATOR")/2, 618-textheight("U")/2, "URMATOR");
        else
            outtextxy(700-textwidth("OK")/2, 618-textheight("O")/2, "OK");
    }

}

void board(){
    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);

    //selectarea imaginii challenge-ului ales
    if (playingStarter)
    readimagefile("starter/starter.jpg",0,0,screenWidth,screenHeight);
    else
        if(playingJunior)
        readimagefile("junior/junior.jpg",0,0,screenWidth,screenHeight);
        else
            if(playingExpert)
            readimagefile("expert/expert.jpg",0,0,screenWidth,screenHeight);
                else
                    if(playingMaster)
                    readimagefile("master/master.jpg",0,0,screenWidth,screenHeight);
                        else
                            if(playingGenerated)
                            readimagefile("generated/generated.jpg",0,0,screenWidth,screenHeight);
    //The shapes:
    setfillstyle(SOLID_FILL, COLOR(0, 105, 148)); //sea_blue

    shape_container(shapes, 101, 79, 400, blue_2, light_blue, 0);
    matrix_coord[0].x=101; matrix_coord[0].y=79;

    shape_container(shapes, 101, 377, 400, blue_2, light_blue, 1);
    matrix_coord[1].x=101; matrix_coord[1].y=377;

    shape_container(shapes, 1085, 79, 400, blue_2, light_blue, 2);
    matrix_coord[2].x=1085; matrix_coord[2].y=79;

    shape_container(shapes, 1085, 377, 400, blue_2, light_blue, 3);
    matrix_coord[3].x=1085; matrix_coord[3].y=377;

    //Back button
    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
    setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
    bar(333, 609, 478, 664);
    if(in_english){
        outtextxy(405-textwidth("BACK")/2, 636-textheight("B")/2, "BACK");
    }
    else{
        outtextxy(405-textwidth("INAPOI")/2, 636-textheight("B")/2, "INAPOI");
    }


    //Hint button
    bar(920, 609, 1075, 664);
    if(in_english){
        outtextxy(997-textwidth("HINT")/2, 636-textheight("H")/2, "HINT");
    }
    else{
        outtextxy(997-textwidth("INDICIU")/2, 636-textheight("I")/2, "INDICIU");
    }


    //Rotate button
    bar(610, 590, 791, 645);
    if(in_english){
        outtextxy(700-textwidth("ROTATE")/2, 618-textheight("R")/2, "ROTATE");
    }
    else{
        outtextxy(700-textwidth("ROTESTE")/2, 618-textheight("R")/2, "ROTESTE");
    }

}

void close_level(){
    for(int i=0;i<4;i++){
        available[i] = true;
        placed_pieces[i] = false;
    }
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            selected_matrix[i][j] = 0;
        }
    }
    for(int i = 0; i < 16; i++){
        challenge[i] = -1;
    }
    is_selected = false;

    in_levels = true; gameStarted = false;

                if (playingStarter)
                    playingStarter=false;
                else
                    if (playingJunior)
                        playingJunior=false;
                    else
                        if (playingExpert)
                            playingExpert=false;
                        else
                            if (playingMaster)
                                playingMaster = false;
                            else
                                if (playingGenerated)
                                    playingGenerated = false;

}

bool all_placed(){
    for(int i = 0; i<4; i++){
        if(placed_pieces[i] == false) return false;
    }
    //placed_pieces[0] = 0;
    return true;
}

bool check_solution(){
    /*cout<<"Board Matrix result: "<<endl;
    for(int k = 0; k < 4; k++){
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                cout<<board_matrix[k][i][j]<<" ";
            }
            cout<<endl;
        }
        cout<<"----------"<<endl;
    }*/
    int challenge_count[8] = {0,0,0,0,0,0,0,0}, board_count[8] = {0,0,0,0,0,0,0,0}; //0 - pirate, 1 - ship wreck, 2 - ship, 3 - treasure, 4 - tentacles
                                            //5 - tower, 6 - ship pirates, 7 - island
    for(int i = 0; i < 16 ; i++){
        if(challenge[i] == 2){
            challenge_count[0]++;
        }
        if(challenge[i] == 3){
            challenge_count[1]++;
        }
        if(challenge[i] == 4){
            challenge_count[2]++;
        }
        if(challenge[i] == 5){
            challenge_count[3]++;
        }
        if(challenge[i] == 6){
            challenge_count[4]++;
        }
        if(challenge[i] == 7){
            challenge_count[5]++;
        }
        if(challenge[i] == 8){
            challenge_count[6]++;
        }
        if(challenge[i] == 9){
            challenge_count[7]++;
        }
    }
    for(int k = 0; k < 4; k++){
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                if(board_matrix[k][i][j] == 2){
                    board_count[0]++;
                }
                if(board_matrix[k][i][j] == 3){
                    board_count[1]++;
                }
                if(board_matrix[k][i][j] == 4){
                    board_count[2]++;
                }
                if(board_matrix[k][i][j] == 5){
                    board_count[3]++;
                }
                if(board_matrix[k][i][j] == 6){
                    board_count[4]++;
                }
                if(board_matrix[k][i][j] == 7){
                    board_count[5]++;
                }
                if(board_matrix[k][i][j] == 8){
                    board_count[6]++;
                }
                if(board_matrix[k][i][j] == 9){
                    board_count[7]++;
                }
            }
        }
    }
    for(int i = 0; i < 8; i++){
        if(challenge_count[i] != board_count[i]) return false;
    }
    return true;
}


//Challenge generator
void generate_challenge(){
    int used_numbers[4] = {-1,-1,-1,-1};
    int k = 0, aux;
    int aux_matrix[3][3];
    bool new_num = false;
    bool rotate_shape = false;
    int rotate_times = 0;
    srand(time(0));
    while (k < 4){
        while(!new_num){

            new_num = true;
            aux = rand() % 4; rotate_shape = rand() % 2; rotate_times = rand() % 6;
            //cout<<"Chose number "<<aux<<endl<<"Checking number:"<<endl;
            for(int i = 0; i < 4; i++){
                //cout<<used_numbers[i]<<" ";
                if(used_numbers[i] == aux) new_num = false;
            }
            //cout<<endl;
            if(new_num){
                used_numbers[k] = aux;
            }
        }
        new_num = false;
        //cout<<"Placed shape "<<aux<<" on square "<<k<<endl;
        solution[k] = aux;
        copy_matrix(aux_matrix,shapes[aux]);
        /*cout<<"The shape: "<<endl;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                cout<<aux_matrix[i][j]<<" ";
            }
            cout<<endl;
        }*/
        /*cout<<"Square before: "<<endl;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                cout<<board_matrix[k][i][j]<<" ";
            }
            cout<<endl;
        }*/
        if(rotate_shape){
            solution_rotation[k] = rotate_times;
            //cout<<"Rotated "<<rotate_times<<":"<<endl;
            for(int l = 0; l < rotate_times; l++){
                rotate90Clockwise(aux_matrix);
            }
        }
        else{
            solution_rotation[k] = 0;
            //cout<<"No rotation"<<endl;
        }
        //cout<<"~~~~~~~~~~~~~~~~~~~"<<endl;
        hide_matrix(board_matrix[k], aux_matrix);
        /*cout<<"Result: "<<endl;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                cout<<board_matrix[k][i][j]<<" ";
            }
            cout<<endl;
        }
        cout<<endl<<"Next number ------------"<<endl;*/
        k++;
    }
    aux = 0;
    for(int k = 0; k < 4; k++){ // Selecteaza elementele pentru challenge
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                if(board_matrix[k][i][j] != 0){
                    challenge[aux] = board_matrix[k][i][j];
                    aux++;
                }
            }
        }
    }

    for(int i = 0; i < 4; i ++){
        cout<<"Solution: Piece "<<solution[i]<<" on square "<<i<<endl;
        cout<<"Rotation: "<<solution_rotation[i]<<endl<<endl;
    }
    refresh_board(4,0,5);

}

void print_challenge(){
    int i = 0;
    while(challenge[i] != -1 && i < 16){
        cout<<challenge[i]<<" ";
        i++;
    }
    if(challenge[0] == -1){
        cout<<"Challenge empty!"<<endl;
    }
    /*cout<<endl<<"Shapes: "<<endl;
    for(int k = 0; k < 4; k++){
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                cout<<shapes[k][i][j]<<" ";
            }
            cout<<endl;
        }
        cout<<endl;
    }*/

}

void display_challenge(int startX, int startY){ //deafult shall be x = 300 y = 42 size: 600 X 90

    setfillstyle(SOLID_FILL, COLOR(252, 243, 134));
    //bar(startX, startY, startX + 610, startY + 90);
    int i = 0; int aux = 1, k = 0;
    while(i < 16 && challenge[i] != -1){
            cout<<"Cheking element "<<i<<" = "<<challenge[i]<<endl;
            if(i >= 8){
                aux = 5;
                k-=8;
            }
            if(challenge[i] == 2){
                readimagefile("aux_img/pirate.jpg",startX+76*k, (startY - 23)*aux, ((startX+76*k)+76), ((startY - 23)*aux + 76));
            }
            if(challenge[i] == 3){
                readimagefile("aux_img/ship_wrecked.jpg",startX+76*k, (startY - 23)*aux, (startX+76*k)+76, (startY - 23)*aux + 76);
            }
            if(challenge[i] == 4){
                readimagefile("aux_img/ship.jpg",startX+76*k, (startY - 23)*aux, (startX+76*k)+76, (startY - 23)*aux + 76);
            }
            if(challenge[i] == 5){
                readimagefile("aux_img/treasure.jpg",startX+76*k, (startY - 23)*aux, (startX+76*k)+76, (startY - 23)*aux + 76);
            }
            if(challenge[i] == 6){
                readimagefile("aux_img/tentacles.jpg",startX+76*k, (startY - 23)*aux, (startX+76*k)+76, (startY - 23)*aux + 76);
            }
            if(challenge[i] == 7){
                readimagefile("aux_img/tower.jpg",startX+76*k, (startY - 23)*aux, (startX+76*k)+76, (startY - 23)*aux + 76);
            }
            if(challenge[i] == 8){
                readimagefile("aux_img/ship_pirate.jpg",startX+76*k, (startY - 23)*aux, (startX+76*k)+76, (startY - 23)*aux + 76);
            }
            if(challenge[i] == 9){
                readimagefile("aux_img/island.jpg",startX+76*k, (startY - 23)*aux, (startX+76*k)+76, (startY - 23)*aux + 76);
            }
        i++; k++;
    }
}
//End

void refresh_challenge(){
    for(int i = 0; i < 16; i++){
        challenge[i] = -1;
    }
}

void won(){
    //cout<<"Entered function!"<<endl;
    if(in_english){
        //cout<<"Got english!"<<endl;
        readimagefile("aux_img/won_en.jpg",0,0,screenWidth,screenHeight);

        //back
        setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
        settextstyle(BOLD_FONT, HORIZ_DIR, 5);
        setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
        bar(333, 609, 478, 664);
        outtextxy(405-textwidth("BACK")/2, 636-textheight("B")/2, "BACK");
    }
    else{
        //cout<<"Got romanian!"<<endl;
        readimagefile("aux_img/won_ro.jpg",0,0,screenWidth,screenHeight);

        //back
        setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
        settextstyle(BOLD_FONT, HORIZ_DIR, 5);
        setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
        bar(333, 609, 478, 664);
        outtextxy(405-textwidth("INAPOI")/2, 636-textheight("I")/2, "INAPOI");
    }
}

void lost(){

    if(in_english){

        readimagefile("aux_img/lost_en.jpg",0,0,screenWidth,screenHeight);

        //back
        setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
        settextstyle(BOLD_FONT, HORIZ_DIR, 5);
        setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
        bar(333, 609, 478, 664);
        outtextxy(405-textwidth("BACK")/2, 636-textheight("B")/2, "BACK");
    }
    else{

        readimagefile("aux_img/lost_ro.jpg",0,0,screenWidth,screenHeight);

        //back
        setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
        settextstyle(BOLD_FONT, HORIZ_DIR, 5);
        setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
        bar(333, 609, 478, 664);
        outtextxy(405-textwidth("INAPOI")/2, 636-textheight("I")/2, "INAPOI");
    }
}

void get_solution(int level){
    if(level == 1){ //starter
        solution[0] = 1; solution[1] = 0; solution[2] = 2; solution[3] = 3;
        solution_rotation[0] = 3; solution_rotation[1] = 2; solution_rotation[2] = 3; solution_rotation[3] = 1;
    }
    else if(level == 2){//junior

    }
    else if(level == 3){//expert

    }
    else if(level == 4){//master

    }
}

void give_hint(){
    bool is_hint = false;
    int square_num; int piece; int nr;
    int aux_mat[3][3];
    for(int i = 0; i < 4; i++){
        if(!placed_pieces[i]){
            square_num = i; piece = solution[square_num];
            if(available[piece]) {
                    is_hint = true;
                    break;
            }
        }
    }
    if(is_hint){
        copy_matrix(aux_mat, shapes[piece]);
        for(int i = 0; i < solution_rotation[square_num]; i++){
            rotate90Clockwise(aux_mat);
        }
        if(square_num == 0){
            placed++;
            nr = piece;
            placed_pieces[0] = true; hide_matrix(board_matrix[0],aux_mat);
            square( matrix_coord[nr].x, matrix_coord[nr].y, 200, true, blue_1);
            draw_shape(aux_mat, 533, 200, 158/3, blue_1);
        }
        if(square_num == 1){
            placed++;
            nr = piece;
            placed_pieces[1] = true; hide_matrix(board_matrix[1],aux_mat);
            square( matrix_coord[nr].x, matrix_coord[nr].y, 200, true, blue_1);
            draw_shape(aux_mat, 710, 200, 158/3, blue_1);
        }
        if(square_num == 2){
            placed++;
            nr = piece;
            placed_pieces[2] = true; hide_matrix(board_matrix[2],aux_mat);
            square( matrix_coord[nr].x, matrix_coord[nr].y, 200, true, blue_1);
            draw_shape(aux_mat, 533, 372, 158/3, blue_1);
        }
        if(square_num == 3){
            placed++;
            nr = piece;
            placed_pieces[3] = true; hide_matrix(board_matrix[3],aux_mat);
            square( matrix_coord[nr].x, matrix_coord[nr].y, 200, true, blue_1);
            draw_shape(aux_mat, 710, 372, 158/3, blue_1);
        }
        hint_available = false;
    }
}

void get_shapes(string filename){
    fstream fi;
    fi.open(filename,ios::in);
    for(int k = 0 ; k < 4; k++){
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                fi>>shapes[k][i][j];
            }
        }
    }
    fi.close();
}

void get_challenge(string levelname){
    fstream fi;
    string aux; aux = levelname+"/challenge.in";
    fi.open(aux,ios::in);
    int i = 0;
    while(i < 16){
        fi>>challenge[i];
        if(challenge[i] == -1) break;
        i++;
    }
    fi.close();
}

bool startGame(){// ciclul principal al jocului unde are loc procesarea logicii
    bool is_done = false;
    bool draw = true; //pentru HOW TO
    bool initiate_challenge = true;
    while(gameOpen){

        getmouseclick(WM_LBUTTONDOWN,mouseX,mouseY);
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

            //nivelul STARTER
            if(in_border(mouseX,mouseY,535, 372, 866, 427)){
                clearmouseclick(WM_LBUTTONDOWN);
                get_shapes("starter/shapes.in");
                gameStarted = true; in_levels = false;
                playingStarter = true;
                refresh_board(4,0,1); get_challenge("starter");
                get_solution(1);
                board();
            }

            //nivelul JUNIOR
            if(in_border(mouseX,mouseY,535, 372+70, 866, 427+70)){
                clearmouseclick(WM_LBUTTONDOWN);
                get_shapes("junior/shapes.in");
                gameStarted = true; in_levels = false;
                playingJunior = true;
                refresh_board(4,0,2); get_challenge("junior");
                get_solution(2);
                board();
            }

            //nivelul EXPERT
            if(in_border(mouseX,mouseY,535, 372+140, 866, 427+140)){
                clearmouseclick(WM_LBUTTONDOWN);
                get_shapes("expert/shapes.in");
                gameStarted = true; in_levels = false;
                playingExpert = true;
                refresh_board(4,0,3); get_challenge("expert");
                get_solution(3);
                board();
            }

            //nivelul MASTER
            if(in_border(mouseX,mouseY,535, 372+210, 866, 427+210)){
                clearmouseclick(WM_LBUTTONDOWN);
                get_shapes("master/shapes.in");
                gameStarted = true; in_levels = false;
                playingMaster = true;
                refresh_board(4,0,4); get_challenge("master");
                get_solution(4);
                board();
            }

            //nivelul Generated
            if(in_border(mouseX, mouseY, 949, 509, 1140, 564)){
                //close_level();
                clearmouseclick(WM_LBUTTONDOWN);
                in_levels = false; gameStarted = true;
                playingGenerated = true;
                read_shapes();
                refresh_board(4,0,5);
                generate_challenge();
                print_challenge();
                board();
                display_challenge(400,42);
            }

            //tutorial
            if(in_border(mouseX,mouseY,534, 257, 865, 311)){
                clearmouseclick(WM_LBUTTONDOWN);
                in_levels = false; in_tutorial = true;
                step = 1;
            }

        }
        if(gameStarted){

            int nr;
            //placed contorizeaza numarul pieselor plasate
            //selectarea figurilor
            if(in_border(mouseX,mouseY, 101, 79, 304, 332) && available[0] && !is_selected){
                nr=0;
                copy_matrix(selected_matrix , shapes[0]);
                shape_container(shapes, 101, 79, 400, blue_2, blue_1, 0);
                is_selected = true;
                available[0] = 0;
            }
            if(in_border(mouseX,mouseY, 101, 377, 304, 630) && available[1] && !is_selected){
                nr=1;
                copy_matrix(selected_matrix , shapes[1]);
                shape_container(shapes, 101, 377, 400, blue_2, blue_1, 1);
                is_selected = true;
                available[1] = 0;
            }
            if(in_border(mouseX,mouseY, 1085, 79, 1338, 332) && available[2] && !is_selected){
                nr=2;
                copy_matrix(selected_matrix , shapes[2]);
                shape_container(shapes, 1085, 79, 400, blue_2, blue_1, 2);
                is_selected = true;
                available[2] = 0;
            }
            if(in_border(mouseX,mouseY, 1085, 377, 1338, 630) && available[3] && !is_selected){
                nr=3;
                copy_matrix(selected_matrix , shapes[3]);
                shape_container(shapes, 1085, 377, 400, blue_2, blue_1, 3);
                is_selected = true;
                available[3] = 0;
            }

            int nrtemp[4]; //pentru a retine locul fiecarei piese cand se pune inapoi

            //plasarea figurilor
            if (is_selected && in_border(mouseX,mouseY, 533, 200, 691, 368) && !placed_pieces[0]){
                placed++;
                nrtemp[0]=nr;
                placed_pieces[0] = true; hide_matrix(board_matrix[0],selected_matrix);
                square( matrix_coord[nr].x, matrix_coord[nr].y, 200, true, blue_1);
                draw_shape(selected_matrix, 533, 200, 158/3, blue_1);
                is_selected = false;

            }
            else
               if (!is_selected && in_border(mouseX,mouseY, 533, 200, 691, 368) && placed_pieces[0]){
                    placed--;
                    if(playingStarter){
                        readimagefile("starter/panou1.jpg", 533, 200, 533+158, 200+158);
                        refresh_board(1,0,1);
                    }
                    if(playingJunior){
                        readimagefile("junior/panou1.jpg", 533, 200, 533+158, 200+158);
                        refresh_board(1,0,2);
                    }
                    if(playingExpert){
                        readimagefile("expert/panou1.jpg", 533, 200, 533+158, 200+158);
                        refresh_board(1,0,3);
                    }
                    if(playingMaster){
                        readimagefile("master/panou1.jpg", 533, 200, 533+158, 200+158);
                        refresh_board(1,0,4);
                    }
                    if(playingGenerated){
                        readimagefile("generated/panou1.jpg", 533, 200, 533+158, 200+158);
                        refresh_board(1,0,5);
                    }
                    shape_container(shapes, matrix_coord[nrtemp[0]].x, matrix_coord[nrtemp[0]].y, 400, blue_2, light_blue, nrtemp[0]);
                    available[nrtemp[0]] = 1;
                    placed_pieces[0] = false;
               }


            if (is_selected && in_border(mouseX,mouseY, 533, 372, 691, 545) && !placed_pieces[2]){
                placed++;
                nrtemp[1]=nr;
                placed_pieces[2] = true; hide_matrix(board_matrix[2],selected_matrix);
                square( matrix_coord[nr].x, matrix_coord[nr].y, 200, true, blue_1);
                draw_shape(selected_matrix, 533, 372, 158/3, blue_1);
                is_selected = false;
            }
            else
                if (!is_selected && in_border(mouseX,mouseY, 533, 372, 691, 545) && placed_pieces[2]){
                    placed--;
                    //readimagefile(img2, 533, 372, 533+158, 372+158);
                    if(playingStarter){
                        readimagefile("starter/panou3.jpg", 533, 372, 533+158, 372+158);
                        refresh_board(1,2,1);
                    }
                    if(playingJunior){
                        readimagefile("junior/panou3.jpg", 533, 372, 533+158, 372+158);
                        refresh_board(1,2,2);
                    }
                    if(playingExpert){
                        readimagefile("expert/panou3.jpg", 533, 372, 533+158, 372+158);
                        refresh_board(1,2,3);
                    }
                    if(playingMaster){
                        readimagefile("master/panou3.jpg", 533, 372, 533+158, 372+158);
                        refresh_board(1,2,4);
                    }
                    if(playingGenerated){
                        readimagefile("generated/panou3.jpg", 533, 372, 533+158, 372+158);
                        refresh_board(1,2,5);
                    }
                    shape_container(shapes, matrix_coord[nrtemp[1]].x, matrix_coord[nrtemp[1]].y, 400, blue_2, light_blue, nrtemp[1]);
                    available[nrtemp[1]] = 1;
                    placed_pieces[2] = false;
               }

            if (is_selected && in_border(mouseX,mouseY, 710, 200, 869, 368) && !placed_pieces[1]){
                placed++;
                nrtemp[2]=nr;
                placed_pieces[1] = true; hide_matrix(board_matrix[1],selected_matrix);
                square( matrix_coord[nr].x, matrix_coord[nr].y, 200, true, blue_1);
                draw_shape(selected_matrix, 710, 200, 158/3, blue_1);
                is_selected = false;
            }
            else
                if (!is_selected && in_border(mouseX,mouseY, 710, 200, 869, 368) && placed_pieces[1]){
                    placed--;
                    //readimagefile(img3, 710, 200, 710+158, 200+158);
                    if(playingStarter) {
                        readimagefile("starter/panou2.jpg", 710, 200, 710+158, 200+158);
                        refresh_board(1,1,1);
                    }
                    if(playingJunior){
                        readimagefile("junior/panou2.jpg", 710, 200, 710+158, 200+158);
                        refresh_board(1,1,2);
                    }
                    if(playingExpert){
                        readimagefile("expert/panou2.jpg", 710, 200, 710+158, 200+158);
                        refresh_board(1,1,3);
                    }
                    if(playingMaster){
                        readimagefile("master/panou2.jpg", 710, 200, 710+158, 200+158);
                        refresh_board(1,1,4);
                    }
                    if(playingGenerated){
                        readimagefile("generated/panou2.jpg", 710, 200, 710+158, 200+158);
                        refresh_board(1,1,5);
                    }
                    shape_container(shapes, matrix_coord[nrtemp[2]].x, matrix_coord[nrtemp[2]].y, 400, blue_2, light_blue, nrtemp[2]);
                    available[nrtemp[2]] = 1;
                    placed_pieces[1] = false;
               }

            if (is_selected && in_border(mouseX,mouseY, 710, 372, 869, 545) && !placed_pieces[3]){
                placed++;
                nrtemp[3]=nr;
                placed_pieces[3] = true; hide_matrix(board_matrix[3],selected_matrix);
                square( matrix_coord[nr].x, matrix_coord[nr].y, 200, true, blue_1);
                draw_shape(selected_matrix, 710, 372, 158/3, blue_1);
                is_selected = false;
            }
            else
                if (!is_selected && in_border(mouseX,mouseY, 710, 372, 869, 545) && placed_pieces[3]){
                    placed--;
                    //readimagefile(img4, 710, 372, 710+158, 372+158);
                    if(playingStarter){
                        readimagefile("starter/panou4.jpg", 710, 372, 710+158, 372+158);
                        refresh_board(1,3,1);
                    }
                    if(playingJunior){
                        readimagefile("junior/panou4.jpg", 710, 372, 710+158, 372+158);
                    }
                    if(playingExpert){
                        readimagefile("expert/panou4.jpg", 710, 372, 710+158, 372+158);
                    }
                    if(playingMaster){
                        readimagefile("master/panou4.jpg", 710, 372, 710+158, 372+158);
                    }
                    if(playingGenerated){
                        readimagefile("generated/panou4.jpg", 710, 372, 710+158, 372+158);
                        refresh_board(1,3,5);
                    }
                    shape_container(shapes, matrix_coord[nrtemp[3]].x, matrix_coord[nrtemp[3]].y, 400, blue_2, light_blue, nrtemp[3]);
                    available[nrtemp[3]] = 1;
                    placed_pieces[3] = false;
               }

            //Hint
            if(in_border(mouseX, mouseY, 970, 609, 1145, 664) && hint_available){
                clearmouseclick(WM_LBUTTONDOWN);
                give_hint();
            }

            //Back
            if (in_border(mouseX, mouseY, 307, 609, 452, 664) && !all_placed()){
                close_level();
                levels();
                hint_available = true;
            }

            //Rotate / Done
            if(!is_done){
                if (in_border(mouseX, mouseY, 628, 590, 773, 645) && placed!=4 && is_selected){
                    rotate90Clockwise(shapes[nr]);
                    copy_matrix(selected_matrix , shapes[nr]);
                    shape_container(shapes, matrix_coord[nr].x, matrix_coord[nr].y, 400, blue_2, blue_1, nr);
                    is_selected = true;
                    available[nr] = 0;
                }
            }
            else{ //Done
                hint_available = true;
                if (in_border(mouseX, mouseY, 628, 590, 773, 645)){
                    if(check_solution()){
                        cout<<"You Won!";
                        won(); win_screen = true;
                    }
                    else{
                        cout<<"You lost!";
                        lost(); lose_screen = true;
                    }
                    //close_level();
                    //levels();
                }
            }

            //Win / Lose
            if(win_screen || lose_screen){ //Back
                if(in_border(mouseX, mouseY, 307, 609, 452, 664)){
                    close_level(); levels();
                    win_screen = false;
                }
            }


            if(!all_placed() && is_done){
                setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
                settextstyle(BOLD_FONT, HORIZ_DIR, 5);
                setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
                is_done = false;
                if(gameStarted){
                    readimagefile("aux_img/img1.jpg",307, 590, 1145, 664);
                    bar(610, 590, 791, 645);
                    if(in_english){
                        outtextxy(700-textwidth("ROTATE")/2, 618-textheight("R")/2, "ROTATE");
                    }
                    else{
                        outtextxy(700-textwidth("ROTESTE")/2, 618-textheight("R")/2, "ROTESTE");
                    }
                    //Back button
                    setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
                    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
                    setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
                    bar(333, 609, 478, 664);
                    if(in_english){
                        outtextxy(405-textwidth("BACK")/2, 636-textheight("B")/2, "BACK");
                    }
                    else{
                        outtextxy(405-textwidth("INAPOI")/2, 636-textheight("B")/2, "INAPOI");
                    }

                    //Hint button
                    bar(920, 609, 1075, 664);
                    if(in_english){
                        outtextxy(997-textwidth("HINT")/2, 636-textheight("H")/2, "HINT");
                    }
                    else{
                        outtextxy(997-textwidth("INDICIU")/2, 636-textheight("I")/2, "INDICIU");
                    }
                }

            }
            if(all_placed() && !is_done){
                setfillstyle(SOLID_FILL, COLOR(152, 65, 21));
                settextstyle(BOLD_FONT, HORIZ_DIR, 6);
                setbkcolor(COLOR(sea_blue[0],sea_blue[1],sea_blue[2]));
                is_done = true;
                bar(307, 590, 1145, 664);
                if(in_english){
                    outtextxy(700-textwidth("DONE")/2, 618-textheight("D")/2, "DONE");
                }
                else{
                    outtextxy(700-textwidth("GATA")/2, 618-textheight("G")/2, "GATA");
                }
            }

        }
        if(in_settings){
            /*if(mouseX > 853 && mouseX < 936 && mouseY > 340 && mouseY < 395){ //music on/off
                clearmouseclick(WM_LBUTTONDOWN);
                if (music==true)
                {
                    music=false;
                    playMusic(music);
                    setfillstyle(SOLID_FILL, COLOR(139, 194, 234));
                    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
                    setbkcolor(COLOR(139, 194, 234));
                    bar(853, 340, 936, 395);
                    outtextxy(895-textwidth("OFF")/2, 367-textheight("O")/2, "OFF");
                }
                else
                {
                    music=true;
                    playMusic(music);
                    setfillstyle(SOLID_FILL, COLOR(139, 194, 234));
                    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
                    setbkcolor(COLOR(139, 194, 234));
                    bar(853, 340, 936, 395);
                    outtextxy(895-textwidth("ON")/2, 367-textheight("O")/2, "ON");
                }
            }*/
            if(in_border(mouseX,mouseY,307, 609, 452, 664)){
                clearmouseclick(WM_LBUTTONDOWN);
                in_settings = false; in_menu = true;
                menu();
            }
            if(in_border(mouseX,mouseY, 853, 446, 936, 501)){
                setfillstyle(SOLID_FILL, COLOR(139, 194, 234));
                settextstyle(BOLD_FONT, HORIZ_DIR, 5);
                setbkcolor(COLOR(139, 194, 234));

                if(in_english) in_english = false;
                else in_english = true;

                if(in_english){
                    bar(853, 446, 936, 501);
                    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
                    outtextxy(895-textwidth("EN")/2, 474-textheight("E")/2, "EN");
                }
                else{
                    bar(853, 446, 936, 501);
                    settextstyle(BOLD_FONT, HORIZ_DIR, 5);
                    outtextxy(895-textwidth("RO")/2, 474-textheight("E")/2, "RO");
                }
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
