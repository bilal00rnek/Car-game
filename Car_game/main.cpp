#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <iostream>
#include <queue>
#include <ctime>
#include <string.h>
#define wHeight 40 // height of the road
#define wWidth 100 // width of the road
#define lineX 45 // x coordinate of the middle line
#define lineLEN 10 // distance of the middle line from the beginning and the end
#define EXITY 35 // coordinate showing the end of the road
#define leftKeyArrow 260 // ASCII code of the left arrow key
#define RightKeyArrow 261 // ASCII code of the right arrow key
#define leftKeyA 97// ASCII code of A
#define RightKeyD 100 // ASCII code of D
#define ESC 27 // // ASCII code of the ESC key
#define ENTER 10 // ASCII code of the ENTER key
#define KEYPUP 259 // ASCII code of the up arrow key
#define KEYDOWN 258 // ASCII code of the down arrow key
#define KEYERROR -1 // ASCII code returned if an incorrect key is pressed
#define SAVEKEY 115 // ASCII code of S
#define levelBound 300 // To increase level after 300 points
#define MAXSLEVEL 5 // maximum level
#define ISPEED 500000 // initial value for game moveSpeed
#define DRATESPEED 100000 // to decrease moveSpeed after each new level
#define MINX 5 // minimum x coordinate value when creating cars
#define MINY 10 // the maximum y coordinate value when creating the cars, then we multiply it by -1 and take its inverse
#define MINH 5 // minimum height when creating cars
#define MINW 5 // minimum width when creating cars
#define SPEEDOFCAR 3 // speed of the car driven by the player
#define YOFCAR 34 // y coordinate of the car used by the player
#define XOFCAR 45 // x coordinate of the car used by the player
#define IDSTART 10 // initial value for cars ID
#define IDMAX 20// maximum value for cars ID
#define COLOROFCAR 3 // color value of the car used by the player
#define POINTX 91 //x coordinate where the point is written
#define POINTY 42 //y coordinate where the point is written
#define MENUX 10 // x coordinate for the starting row of the menus
#define MENUY 5 // y coordinate for the starting row of the menus
#define MENUDIF 2 // difference between menu rows
#define MENUDIFX 20 // difference between menu columns
#define MENSLEEPRATE 200000 // sleep time for menu input
#define GAMESLEEPRATE 250000 // sleep time for player arrow keys
#define EnQueueSleep 1 // EnQueue sleep time
#define DeQueueSleepMin 2 // DeQueue minimum sleep time
#define numOfcolors 4 // maximum color value that can be selected for cars
#define maxCarNumber 5 // maximum number of cars in the queue
#define numOfChars 3 // maximum number of patterns that can be selected for cars
#define settingMenuItem 2 // number of options in the setting menu
#define mainMenuItem 6 // number of options in the main menu
using namespace std;
typedef struct Car{ //
    int ID;
    int x;
    int y;
    int height;
    int width;
    int speed;
    int clr;
    bool isExist;
    char chr;
}Car;
typedef struct Game{
    int leftKey;
    int rightKey;
    queue<Car> cars;
    bool IsGameRunning;
    bool IsSaveCliked;
    int counter;
    pthread_mutex_t mutexFile;
    Car current;
    int level;
    int moveSpeed;
    int points;
}Game;


Game playingGame; // Global variable used for new game
const char *gameTxt =  "game.txt";
const char *CarsTxt =  "cars.txt";
const char *pointsTxt =  "points.txt";
//Array with options for the Setting menu
const char *settingMenu[50] = {"Play with < and > arrow keys","Play with A and D keys"};
const char carShape[3]={'#','*','+'};
//Array with options for the Main menu
const char *mainMenu[50] = {"New Game","Load the last game","Instructions", "Settings","Points","Exit"};


void drawCar(Car c, int type, int direction); //prints or remove the given car on the screen
void printWindow(); //Draws the road on the screen
void *newGame(void *); // manages new game
void *enqueue(void *);
void *dequeue(void *);
void *MoveCar(void* car);
void initGame(); // Assigns initial values to all control parameters for the new game
void initWindow(); //Creates a new window and sets I/O settings
int createMainMenu();
void createInstructions();
int createSettings();
int generateRandomNumber(int upperBound ,int lowerBound);
void WritePoints(int point);
void ReadAndPrintPoints();
void WriteGameInformation(Game game);
Game ReadGameInformation();
void *WriteCarInformation(void* car);
void *ReadCarInformationAndMove(void*);

Car generateCar();


int main()//Common
{
   playingGame.leftKey = leftKeyArrow;
   playingGame.rightKey = RightKeyArrow;

   initWindow();

   int option;
   while(true)
   {

       option = createMainMenu();

       if(option==5)
       {
           initGame();
           pthread_t th1; //create new thread
           pthread_t th2;
           pthread_t th3;

           pthread_create(&th1, NULL, newGame,NULL);// Run newGame function with thread
           pthread_create(&th2, NULL, enqueue,NULL);
           pthread_create(&th3, NULL, dequeue,NULL);

           pthread_join(th1, NULL); //Wait for the thread to finish, when the newGame function finishes, the thread will also finish.*/
           pthread_join(th2, NULL);
           pthread_join(th3, NULL);

       }
       else if(option==7)
       {
           initGame();
           playingGame=ReadGameInformation();
           if(playingGame.IsGameRunning==false){
               playingGame.IsGameRunning=true;
               playingGame.IsSaveCliked=false;
               playingGame.cars=queue<Car>();


               pthread_t th1;
               pthread_t th2;
               pthread_t th3;
               pthread_t th4;

               pthread_create(&th1, NULL, newGame,NULL);
               pthread_create(&th2, NULL, enqueue,NULL);
               pthread_create(&th3, NULL, dequeue,NULL);
               pthread_create(&th4, NULL,ReadCarInformationAndMove,NULL);

               pthread_join(th1, NULL);
               pthread_join(th2, NULL);
               pthread_join(th3, NULL);
               pthread_join(th4, NULL);
           }
           else{
               pthread_t th1;
               pthread_t th2;
               pthread_t th3;

               pthread_create(&th1, NULL, newGame,NULL);
               pthread_create(&th2, NULL, enqueue,NULL);
               pthread_create(&th3, NULL, dequeue,NULL);

               pthread_join(th1, NULL);
               pthread_join(th2, NULL);
               pthread_join(th3, NULL);
           }
       }
       else if(option==9)
       {
           createInstructions();
       }
       else if(option==11)
       {
           int optionSettings = createSettings();
           if(optionSettings==MENUY){
               playingGame.leftKey = leftKeyArrow;
               playingGame.rightKey = RightKeyArrow;
           }
           else if(optionSettings==MENUY+MENUDIF){
               playingGame.leftKey = leftKeyA;
               playingGame.rightKey = RightKeyD;
           }

       }
       else if(option==13)
       {
           ReadAndPrintPoints();
       }
       else if(option==15)
       {
           endwin();
           break;
       }
   }
    return 0;
}


Car generateCar()//Tamer
{
    Car car ;
    srand(time(NULL));
    int x=generateRandomNumber(90,5);
    int y=generateRandomNumber(0,-10);
    int height=generateRandomNumber(7,5);
    int width=generateRandomNumber(7,5);
    int color=generateRandomNumber(4,1);
    int shape=generateRandomNumber(2,0);

    car.ID=playingGame.counter;
    car.x=x;
    if(car.x == lineX){
        car.x=generateRandomNumber(89,lineX+1);
    }
    else if(lineX-car.x <= car.width)
    {
        if(lineX-car.x<=car.width/2){
            car.x=generateRandomNumber(89,lineX+1);
        }
        else{
            car.x=generateRandomNumber(lineX-(width+1),4);
        }
    }

    (car).y=y;
    (car).height=height;
    (car).width=width;
    (car).speed=height/2;
    (car).clr=color;
    (car).isExist=false;
    (car).chr=carShape[shape];
    return car;
}

int generateRandomNumber(int upperBound ,int lowerBound)//Tamer
{
    int randomNumber=rand() % (upperBound - lowerBound +1) + lowerBound;
    return randomNumber;
}

void initGame()
{
    playingGame.cars = queue<Car>();
    playingGame.counter =IDSTART;
    playingGame.mutexFile = PTHREAD_MUTEX_INITIALIZER; //assigns the initial value for the mutex
    playingGame.level = 1;
    playingGame.moveSpeed = ISPEED;
    playingGame.points = 0;
    playingGame.IsSaveCliked = false;
    playingGame.IsGameRunning = true;
    playingGame.current.ID = IDSTART-1;
    playingGame.current.height = MINH;
    playingGame.current.width = MINW;
    playingGame.current.speed = SPEEDOFCAR;
    playingGame.current.x = XOFCAR;
    playingGame.current.y = YOFCAR;
    playingGame.current.clr = COLOROFCAR;
    playingGame.current.chr = '*';
}


int createSettings()//Bilal
{
    sleep(1.5);
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_RED,COLOR_BLACK);
    int x=MENUX;
    int y=MENUY;

    attron(COLOR_PAIR(2));
    mvprintw(y,x-2,"->");
    mvprintw(y,x,settingMenu[0]);
    y+=MENUDIF;
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(1));
    mvprintw(y,x,settingMenu[1]);
    attroff(COLOR_PAIR(1));
    refresh();

    y=MENUY;

    while(true)
    {
        int arrow=getch();
        usleep(MENSLEEPRATE);

        if(arrow == KEYDOWN && y!=MENUY+2){
            attron(COLOR_PAIR(1));
            mvprintw(y,x-2,"  ");
            mvprintw(y,x,settingMenu[0]);
            attroff(COLOR_PAIR(1));

            y+=MENUDIF;
            attron(COLOR_PAIR(2));
            mvprintw(y,x-2,"->");
            mvprintw(y,x,settingMenu[1]);
            attroff(COLOR_PAIR(2));
            refresh();
        }
        else if(arrow == KEYPUP && y!=MENUY){
            attron(COLOR_PAIR(1));
            mvprintw(y,x-2,"  ");
            mvprintw(y,x,settingMenu[1]);
            attroff(COLOR_PAIR(1));

            y-=MENUDIF;
            attron(COLOR_PAIR(2));
            mvprintw(y,x-2,"->");
            mvprintw(y,x,settingMenu[0]);
            attroff(COLOR_PAIR(2));
            refresh();
        }
        else if(arrow == ENTER){
            clear();
            refresh();
            sleep(1.5);
            return y;
        }
    }
}


void createInstructions()//Bilal
{
    sleep(1.5);
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    int x=MENUX;
    int y=MENUY;

    attron(COLOR_PAIR(1));
    mvprintw(y,x,"< or A: moves the car to the left");
    mvprintw(y+2,x,"> or D: moves the car to the right");
    mvprintw(y+4,x,"ESC: exits the game without saving");
    mvprintw(y+6,x,"S: saves and exits the game");
    attroff(COLOR_PAIR(1));
    refresh();
    sleep(5);
    clear();
    refresh();
    sleep(1.5);
}


int createMainMenu()//Bilal
{
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_RED,COLOR_BLACK);
    int x=MENUX;
    int y=MENUY;

    attron(COLOR_PAIR(2));
    mvprintw(y,x-2,"->");
    mvprintw(y,x,mainMenu[0]);
    y+=MENUDIF;
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(1));
    for(int i=1;i<mainMenuItem;i++)
    {
        mvprintw(y,x,mainMenu[i]);
        y+=MENUDIF;
    }
    attroff(COLOR_PAIR(1));
    refresh();
    y=MENUY;

    int index=0;
    while(true)
    {
        int arrow=getch();
        usleep(MENSLEEPRATE);
        if(arrow == KEYDOWN && y!=MENUY+10){
            attron(COLOR_PAIR(1));
            mvprintw(y,x-2,"  ");
            mvprintw(y,x,mainMenu[index]);
            attroff(COLOR_PAIR(1));

            y+=MENUDIF;
            index++;
            attron(COLOR_PAIR(2));
            mvprintw(y,x-2,"->");
            mvprintw(y,x,mainMenu[index]);
            attroff(COLOR_PAIR(2));
            refresh();

        }
        else if(arrow == KEYPUP && y!=MENUY){
            attron(COLOR_PAIR(1));
            mvprintw(y,x-2,"  ");
            mvprintw(y,x,mainMenu[index]);
            attroff(COLOR_PAIR(1));

            y-=MENUDIF;
            index--;
            attron(COLOR_PAIR(2));
            mvprintw(y,x-2,"->");
            mvprintw(y,x,mainMenu[index]);
            attroff(COLOR_PAIR(2));
            refresh();
        }
        else if(arrow == ENTER){
            clear();
            refresh();
            return y;
        }
    }

}


void *enqueue(void *)//Tamer
{
    while(playingGame.IsGameRunning)
        {
            Car car = generateCar();

            if(playingGame.cars.size()<=maxCarNumber)
            {
                playingGame.cars.push(car);
                playingGame.counter++;

                if(playingGame.counter==IDMAX){
                    playingGame.counter=IDSTART;
                }
            }
            sleep(1);
        }
}

void *dequeue(void *)//Tamer
{
    sleep(3);
    int sleepValue[]={2,4};
    srand(time(NULL));
    while(playingGame.IsGameRunning)
    {
        Car *car = (Car*)calloc(1,sizeof(Car));
        *car= playingGame.cars.front();
        (*car).isExist=true;
        pthread_t th;
        pthread_create(&th,NULL,MoveCar,(void*)car);
        playingGame.cars.pop();
        int index =rand() % 2 ;
        sleep(sleepValue[index]);
    }
}


void *MoveCar(void* car)//Tamer
{
    Car* car1 =(Car*)car;
    srand(time(NULL));
    bool checkAccident=false;
    while(playingGame.IsGameRunning && (*car1).isExist)
    {
         drawCar(*car1,1,0);
         (*car1).y+= 1 + rand() % (*car1).speed;
         drawCar(*car1,2,0);

         int distancex = (*car1).x-playingGame.current.x;
         int distancey = ((*car1).y + (*car1).height)-playingGame.current.y;
         if(distancex<0){
             distancex=(-1) * distancex;
         }
         if(distancex<(*car1).width-1 && distancey>=0){
             checkAccident=true;
             (*car1).isExist=false;
             break;
         }
         else if(car1->y > EXITY)
         {
            (*car1).isExist = false;
            break;
         }
         usleep(playingGame.moveSpeed);
    }
    if(checkAccident){
        playingGame.IsGameRunning=false;
        clear();
        refresh();
        WritePoints(playingGame.points);
    }
    if(playingGame.IsGameRunning){
        playingGame.points += (car1->height) * (car1->width);
        char points[10];
        sprintf(points,"Point : %d",playingGame.points);
        mvprintw(POINTY,POINTX,points);
        drawCar(*car1,1,0);
        refresh();
        if(playingGame.level<=5){
            if(playingGame.points>=300 && playingGame.points<600){
                playingGame.level=2;
                playingGame.moveSpeed = ISPEED-100000;
            }
            else if(playingGame.points>=600 && playingGame.points<900){
                playingGame.level=3;
                playingGame.moveSpeed = ISPEED-200000;
            }
            else if(playingGame.points>=900 && playingGame.points<1200){
                playingGame.level=4;
                playingGame.moveSpeed = ISPEED-300000;
            }
            else if(playingGame.points>=1200 && playingGame.points<1500){
                playingGame.level=5;
                playingGame.moveSpeed = ISPEED-400000;
            }
        }
    }
    if(playingGame.IsSaveCliked && (*car1).isExist)
    {
        pthread_t th;
        pthread_create(&th,NULL,WriteCarInformation,(void*)car1);
        pthread_join(th,NULL);
    }
}

void *newGame(void *)//Bilal
{
    printWindow();
    drawCar(playingGame.current,2,1);
    refresh(); // Draw the car the player is driving on the screen
    int key;
    while (playingGame.IsGameRunning) { //continue until the game is over
            key = getch(); //Get input for the player to press the arrow keys
            if (key != KEYERROR) {
                 if (key == playingGame.leftKey && playingGame.current.x>3) { // If the left  key is pressed
                        drawCar(playingGame.current,1,1); // removes player's car from screen
                        playingGame.current.x-=playingGame.current.speed; // update position
                        drawCar(playingGame.current,2,1); // draw player's car with new position
                }
                else if(key == playingGame.rightKey && playingGame.current.x<90){
                        drawCar(playingGame.current,1,1); // removes player's car from screen
                        playingGame.current.x+=playingGame.current.speed; // update position
                        drawCar(playingGame.current,2,1);
                }
                else if(key == ESC){
                        playingGame.IsGameRunning = false;
                        clear();
                        refresh();
                        sleep(1.5);
                }
                else if(key == SAVEKEY){
                    playingGame.IsGameRunning = false;
                    playingGame.IsSaveCliked = true;
                    pthread_mutex_lock(&playingGame.mutexFile);
                    FILE *file=fopen(CarsTxt,"wb");
                    fclose(file);
                    pthread_mutex_unlock(&playingGame.mutexFile);
                    WriteGameInformation(playingGame);
                    clear();
                    refresh();
                    sleep(1.5);
                }
            }
         usleep(GAMESLEEPRATE); // sleep
        }
}


void initWindow()
{
	initscr();            // initialize the ncurses window
	start_color();        // enable color manipulation
	keypad(stdscr, true); // enable the keypad for the screen
	nodelay(stdscr, true);// set the getch() function to non-blocking mode
	curs_set(0);          // hide the cursor
	cbreak();             // disable line buffering
	noecho();             // don't echo characters entered by the user
	clear();              // clear the screen
    sleep(1);
}


void printWindow()//Bilal
{
    sleep(1.5);
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_RED,COLOR_BLACK);
    for (int i = 1; i < wHeight - 1; ++i) {
		//mvprintw: Used to print text on the window, paramters order: y , x , string
        mvprintw(i, 2, "*"); //left side of the road
        mvprintw(i, 0, "*");
        mvprintw(i, wWidth - 1, "*");// right side of the road
        mvprintw(i, wWidth - 3, "*");
    }
    for (int i = lineLEN; i < wHeight -lineLEN ; ++i) { //line in the middle of the road
        mvprintw(i, lineX, "#");
    }
    char text[20];
    sprintf(text,"Point : %d",playingGame.points);
    mvprintw(POINTY,POINTX,text);

    for(int i = 5;i<=25;i+=10)
    {
        attron(COLOR_PAIR(1));
        mvprintw(i, wWidth+6, "*");
        mvprintw(i+1, wWidth+5, "*");
        mvprintw(i+1, wWidth+7, "*");
        mvprintw(i+2, wWidth+4, "*");
        mvprintw(i+2, wWidth+6, "*");
        mvprintw(i+2, wWidth+8, "*");
        attroff(COLOR_PAIR(1));

        attron(COLOR_PAIR(2));
        mvprintw(i+3, wWidth+6, "#");
        mvprintw(i+4, wWidth+6, "#");
        attroff(COLOR_PAIR(2));
    }

}


void drawCar(Car c, int type, int direction )
{
	//If the user does not want to exit the game and the game continues
    if(playingGame.IsSaveCliked!=true && playingGame.IsGameRunning==true)
    {
            init_pair(c.ID, c.clr, 0);// Creates a color pair: init_pair(short pair ID, short foregroundcolor, short backgroundcolor);
            //0: Black (COLOR_BLACK)
			//1: Red (COLOR_RED)
			//2: Green (COLOR_GREEN)
			//3: Yellow (COLOR_YELLOW)
			//4: Blue (COLOR_BLUE)
			attron(COLOR_PAIR(c.ID));//enable color pair
            char drawnChar;
            if (type == 1 )
               drawnChar = ' '; // to remove car
            else
               drawnChar= c.chr; //  to draw char
		    //mvhline: used to draw a horizontal line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
			 mvhline(c.y, c.x, drawnChar, c.width);// top line of rectangle
             mvhline(c.y + c.height - 1, c.x, drawnChar, c.width); //bottom line of rectangle
            if(direction == 0){// If it is any car on the road
                mvhline(c.y + c.height, c.x, drawnChar, c.width);
            }
            else {//player's card
                mvhline(c.y -1, c.x, drawnChar, c.width);
            }
		    //mvvline: used to draw a vertical line in the window
			//shallow. : mvhline(int y, int x, chtype ch, int n)
			//y: horizontal coordinate
			//x: vertical coordinate
			//ch: character to use
			//n: Length of the line
            mvvline(c.y, c.x, drawnChar, c.height); //left line of rectangle
            mvvline(c.y, c.x + c.width - 1, drawnChar, c.height); //right line of rectangle
            char text[5];
            if (type == 1 )
                sprintf(text,"  "); //to remove point
            else
                 sprintf(text,"%d",c.height * c.width); // to show car's point in rectangle
            mvprintw(c.y+1, c.x +1, text);// display car's point in rectangle
            attroff(COLOR_PAIR(c.ID));// disable color pair
    }
}

void WritePoints(int point)//Bilal
{
    FILE *file = fopen(pointsTxt,"a+");
    fprintf(file,"%d\n",point);
    fclose(file);
}

void ReadAndPrintPoints()//Bilal
{
    sleep(1.5);
    int x=MENUX;
    int y=MENUY;
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    attron(COLOR_PAIR(1));
    FILE *file=fopen(pointsTxt,"r+");
    if(file == NULL){
       mvprintw(y,x,"No Points...");
       refresh();
    }
    else{
        int point;
        int gameNumber = 1;
        while(fscanf(file,"%d",&point)==1){
            char text[20];
            sprintf(text,"Game %d : %d",gameNumber,point);
            mvprintw(y,x,text);
            y+=MENUDIF;
            if(y==23){
                y=MENUY;
                x+=MENUDIFX;
            }
            gameNumber++;
        }
        refresh();
    }
    fclose(file);
    attroff(COLOR_PAIR(1));
    sleep(5);
    clear();
    refresh();
    sleep(1.5);
}

void WriteGameInformation(Game game)//Tamer
{
    FILE *file = fopen(gameTxt,"wb");
    fwrite(&game,sizeof(Game),1,file);
    fclose(file);
}
Game ReadGameInformation()//Tamer
{
    FILE *file = fopen(gameTxt,"rb+");
    Game playingGameSaved;
    if(file == NULL){
       return playingGame;
    }
    else{
        fread(&playingGameSaved,sizeof(Game),1,file);
        fclose(file);
        return playingGameSaved;
    }
}

void *WriteCarInformation(void* car)//Tamer
{
     Car* car1=(Car*) car;
     pthread_mutex_lock(&playingGame.mutexFile);
     FILE *file = fopen(CarsTxt,"ab+");
     fwrite(car1,sizeof(Car),1,file);
     fclose(file);
     pthread_mutex_unlock(&playingGame.mutexFile);
}
void *ReadCarInformationAndMove(void*)//Tamer
{
    FILE *file = fopen(CarsTxt,"rb+");
    while(!feof(file)){
        Car *car = (Car*)calloc(1,sizeof(Car));
        fread(car, sizeof(Car),1,file);
        pthread_t th;
        pthread_create(&th,NULL,MoveCar,(void*)car);
        usleep(250000);
    }
    fclose(file);

}

