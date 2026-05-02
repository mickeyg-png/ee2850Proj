#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ncurses.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>

#define WIDTH 50
#define HEIGHT WIDTH / 2
#define MAX_LEN WIDTH * HEIGHT

int DELAY = 50; // in ms
int snakeLen = 1;
int highScore, leaderBoard[5] = {0, 0, 0, 0, 0}, MAX_Y, MAX_X, SNAKE_COLOR = 5, WALL_COLOR = 8, APPLE_COLOR = 2, strIndex = -1, Colors, Login = -1;
bool start = true;
int centerX, centerY;
char profList[99][19], profLead[5][4] = {"NaN", "NaN", "NaN", "NaN", "NaN"};
char colorList[99][8] = {"Backgr", "Red", "Orange", "Yellow", "Green", "Blue", "Purple", "White"};
int colorNums[99] = {0,999000000,999646000,999999000,999000,999, 500000500, 999999999};
double startTime[5];
int score = 0;
int fireNum = 0;
bool fireMode = false;
bool firstTime = true;

struct snake {
    int x, y, prevX, prevY;
};

struct snake snakeArr[MAX_LEN];

typedef struct {
    int x, y;
} fire;

fire fireArr[MAX_LEN];  

typedef struct {
    int x,y;
} apple;

apple applePos;

int colorCh[3] = {0, 0, 0};

bool getTime(double aim, int hold) {
    bool t_Check;
    struct timeval t;
    gettimeofday(&t, NULL);
    if (aim == 0) {
        startTime[0] = (double) (t.tv_sec + t.tv_usec / 1e6);
        for (int i = 1; i < 5; i++) startTime[i] = startTime[0];
        t_Check = false;
    } else {
        double currTime;
        currTime = (double) (t.tv_sec + t.tv_usec / 1e6);
        t_Check = (currTime - startTime[hold]) > aim;
        if (t_Check) startTime[hold] = currTime;
    }
    return t_Check;
}

void colorChange(int color, int CLR, int pos, char *text, int x, int y) {
    double RAIN_DEL[3]  = {.05, .1, .25};
    if (CLR == 100) {
        if (getTime(RAIN_DEL[color], color)) colorCh[color]++;
        CLR = (colorCh[color] + pos / ((color == 0) ? 4 : color)) % 6 + 2;
    }
    int change = (text == NULL) ? (CLR - 1) : 0;
    if (CLR == SNAKE_COLOR) {
    }
    init_pair(CLR, CLR - 1, change);
    attron(COLOR_PAIR(CLR));
    if (text == NULL) {
        if (color != 0) {
            mvprintw(centerY - HEIGHT / 2 + y, centerX - WIDTH / 2 + x, " ");
        } else {
            mvprintw(centerY - HEIGHT / 2, centerX - WIDTH / 2 + pos, "#");
            mvprintw(centerY + HEIGHT / 2 + 1, centerX + WIDTH / 2 - pos, "#");
            if (pos <= HEIGHT) {
                mvprintw(centerY + HEIGHT / 2 - pos + 1, centerX - WIDTH / 2, "#");
                mvprintw(centerY - HEIGHT / 2 + pos, centerX + WIDTH / 2, "#");
            }
        }
    } else {
        mvprintw(centerY + y, centerX + x, "%s", text);
    }
    attroff(COLOR_PAIR(CLR));
}


void newApple() {
    applePos.x = rand() % WIDTH;
    applePos.y = rand() % HEIGHT;
    if (applePos.x == 0 || applePos.x == WIDTH) applePos.x += ((applePos.x == 0) ? 1 : -1);
    if (applePos.y == 0 || applePos.y == HEIGHT) applePos.y += ((applePos.y == 0) ? 1 : -1);
}

void leaderboard() {
    FILE *HOLD;
    HOLD = fopen("junk.txt", "w"); //This bullshit is the only thing that makes it work IDFK IM CRASHING OUT!!
    fclose(HOLD);
    for (int i = 0; i < strIndex + 1; i++) {
        char *scores = profList[i] + 3;
        char *junk;
        strtol(scores, &junk, 10);
        scores = junk + 1;
        strtol(scores, &junk, 10);
        scores = junk + 1;
        int check, keep;
        char Keep[4], Check[4];
        strncpy(Check, profList[i], 3);
        check = strtol(scores, &junk, 10) * 100;
        fprintf(HOLD, "%s: %d", Check, check);
        for (int j = 0; j <= 4; j++) {
            if (leaderBoard[j] < check) {
                if (leaderBoard[j] == 0) {
                    leaderBoard[j] = check;
                    strncpy(profLead[j], Check, 3);
                    break;
                }
                keep = leaderBoard[j];
                strncpy(Keep, profLead[j], 3);
                leaderBoard[j] = check;
                strncpy(profLead[j], Check, 3);
                check = keep;
                strcpy(Check, Keep);
            } else if (strcmp(profLead[j], Check) == 0) {
                leaderBoard[j] = check;
                break;
            }
        }
    }
}

void newFire() {
    do {
        fireArr[fireNum].x = rand() % WIDTH;
        fireArr[fireNum].y = rand() % HEIGHT;
        if (fireArr[fireNum].x == 0 || fireArr[fireNum].x == WIDTH) fireArr[fireNum].x += ((fireArr[fireNum].x == 0) ? 1 : -1);
        if (fireArr[fireNum].y == 0 || fireArr[fireNum].y == HEIGHT) fireArr[fireNum].y += ((fireArr[fireNum].y == 0) ? 1 : -1); 
    } while(fireArr[fireNum].x == snakeArr[0].x && fireArr[fireNum].y == snakeArr[0].y);
    fireNum++;
}

int startMenu() {
    clear();
    mvprintw(centerY, centerX - 10, "1. Choose Gamemode");
    mvprintw(centerY + 1, centerX - 10, "2. Options   ");
    mvprintw(centerY + 2, centerX -10, "3. Change account");
    mvprintw(centerY + 3, centerX - 10, "4. Quit      ");
    leaderboard();
    for (int i = 0; i < 5; i++) mvprintw(centerY - 20 + i, centerX - 6, "%d. %s: %d", i + 1, profLead[i], leaderBoard[i]);
    refresh();
    char choice;
    do {
        colorChange(2, SNAKE_COLOR, 0, "Snake", 20, 0);
        colorChange(0, WALL_COLOR, 1, "Wall", 20, 1);
        colorChange(1, APPLE_COLOR, 2, "Apple", 20, 2);
        Sleep(50);
        choice = getch();
    } while (!isdigit(choice));
    int picked = choice - '0';
    if (picked == 1 || picked == 2 || picked == 3 || picked == 4) {
        return picked;
    } else {
        return startMenu();
    }
}

void giveColors(int num) {
    FILE *color_list;
    FILE *color_nums;
    color_list = fopen("colors.txt", "w");
    color_nums = fopen("numcol.txt", "w");
    for (int i = 0; i < num + 1; i++) {
        fprintf(color_list, "%s\n", colorList[i]);
        fprintf(color_nums, "%d\n", colorNums[i]);
    }
    fclose(color_list);
    fclose(color_nums);
}

int getColors() {
    FILE *color_list;
    FILE *color_nums;
    int colorInd;
    if (color_nums = fopen("numcol.txt", "r")) { 
        if (color_list = fopen("colors.txt", "r")) {
            colorInd = -1;
            char check[2];
            do {
                colorInd++;
                fscanf(color_list, "%s", &colorList[colorInd]);
                fscanf(color_nums, "%d", &colorNums[colorInd]);
                sprintf(check, "%d", colorNums[colorInd]);
            } while (strlen(colorList[colorInd]) != 0 && colorInd < 100 && isdigit(check[0]));
            fclose(color_list);
            fclose(color_nums);
        } else colorInd = 8;
    } else colorInd = 8;
    int red, green, blue;
    for (int i = 0; i < colorInd; i++) {
        red = colorNums[i] / 1e6;
        green = (colorNums[i] - red * 1e6) / 1e3;
        blue = colorNums[i] - red * 1e6 - green * 1e3;
        init_color(i, red, green, blue);
        init_pair(i+1, i, i);
    }
    return colorInd;
}

void getRGB(int colorNum, int change) {
    clear();
    mvprintw(centerY + 2, centerX - 4, "R   G   B    (Max value is 255)");
    int red, green, blue;
    char inp;
    for (int i = 0; i < 3; i++) {
        bool two = false, five = false;
        char *garbage;
        int cnt = 0;
        char newCol[4] = "\0";
        do {
            mvprintw(centerY + 3, centerX - 5 + 4 * i, "%s", newCol);
            refresh();
            inp = getchar();
            if (inp == '-') {
                newCol[cnt - 1] = ' ';
                cnt = ((cnt == 0) ? 0 : cnt - 1);
            } else if (tolower(inp) == 'x') {
                return;
            } else if (isdigit(inp)) {
                int holdInp = inp - '0';
                if (cnt == 0 && holdInp < 3) {
                    if (holdInp == 2) {
                        two = true;
                    }
                    newCol[cnt] = inp;
                    cnt++;
                } else if (cnt == 1 && holdInp == 5) {
                    newCol[cnt] = '5';
                    five = true;
                    cnt++;
                } else if (cnt != 0 || holdInp < 2) {
                    if (((two || five) ? holdInp < 6 : true)) {
                        newCol[cnt] = inp;
                        cnt++;
                    }
                }
            }
        } while (cnt < 3 && inp != '\r');
        mvprintw(centerY + 3, centerX - 5 + 4 * i, "%s", newCol);
        if (i == 0) red = strtol(newCol, &garbage, 10);
        if (i == 1) green = strtol(newCol, &garbage, 10);
        if (i == 2) blue = strtol(newCol, &garbage, 10);
    }
    red = (int) ((red / 255.0) * 1000);
    blue = (int) ((blue / 255.0) * 1000);
    green = (int) ((green / 255.0) * 1000);
    colorNums[change] = red * 1e6 + green * 1e3 + blue;
    change != 0 ? giveColors(colorNum) : giveColors(colorNum - 1);
}

void optionMenu() {
    clear();
    int colorNum = getColors();
    mvprintw(centerY, centerX - 10, "1. Change Snake Color");
    mvprintw(centerY + 1, centerX - 10, "2. Change Wall Color");
    mvprintw(centerY + 2, centerX - 10, "3. Change Apple Color");
    mvprintw(centerY + 3, centerX - 10, "4. Add a new color");
    mvprintw(centerY + 4, centerX - 10, "5. Change the speed");
    mvprintw(centerY + 5, centerX - 10, "6. Change the background color");
    mvprintw(centerY + 6, centerX - 10, "7. Restore Default Settings");
    refresh();
    char choice;
    do {
        choice = tolower(getchar());
    } while (!isdigit(choice) && choice != 'x');
    if (choice == 'x') return;
    int picked = choice - '0';
    clear();
    if (picked == 5) {
        while (true) {
            clear();
            mvprintw(centerY, centerX - 28, "The current snake speed is %d ms, input your new speed", DELAY);
            refresh();
            int hold = DELAY;
            scanf("%d", &DELAY);
            mvprintw(centerY + 1, centerX - 17, "Inputted: %d, Press enter to save", DELAY);
            mvprintw(centerY + 2, centerX - 9, "Press X to change");
            refresh();
            char choice; 
            while (getchar() == '\r');
            do {
                choice = tolower(getchar());
            } while (choice != '\r' && choice != 'x');
            if (choice == '\r') break;
            DELAY = hold;
        }
    } else if (picked == 7) {
        mvprintw(centerY, centerX - 5, "Snake: Green");
        mvprintw(centerY, centerX - 5, "Wall: White");
        mvprintw(centerY, centerX - 5, "Apple: Red");
        mvprintw(centerY, centerX - 5, "Speed: 50");
        SNAKE_COLOR = 2;
        WALL_COLOR = 7;
        APPLE_COLOR = 1;
        DELAY = 50;
        getchar();
    } else if (picked == 4) {
        clear();
        mvprintw(centerY - 5, centerX + 30, "The current colors are: ");
        for (int i = 1; i < colorNum; i++) {
            init_pair(i+1, i, 0);
            attron(COLOR_PAIR(i + 1));
            if (i < 26) {
                mvprintw(centerY - 5 + i, centerX + 30, "%d. %s", i, colorList[i]);
            } else {
                mvprintw(centerY - 5 + i, centerX + 40, "%d. %s", i, colorList[i]);
            }
            attroff(COLOR_PAIR(i + 1));
        }
        mvprintw(centerY, centerX - 13, "Input the name of the color");
        int cnt = 0;
        char colName[8] = "";
        do {
            mvprintw(centerY + 2, centerX - 1, "%s", colName);
            refresh();
            char inp = getchar();
            if (inp == '\r') break;
            if (inp == '-') {
                colName[cnt] = '_';
                cnt = ((cnt == 0) ? 0 : cnt - 1);
            }
            if (cnt == 0) {
                inp = toupper(inp);
            } else {
                inp = tolower(inp);
            }
            if (isalpha(inp)) {
                    colName[cnt] = inp;
                    cnt++;
            }
        } while (cnt < 7);
        strcpy(colorList[colorNum], colName);
        getRGB(colorNum, colorNum);
        for (int i = 0; i < colorNum; i++) init_pair(i+1, i, i);
    } else if (picked == 6) {
        getRGB(colorNum, 0);
        getColors();
    } else if (picked == 1 || picked == 2 || picked || 3) {
        mvprintw(centerY - 3, centerX - 15, "Select color, press X to return");
        mvprintw(centerY - 4, centerX - 10, "Your colors are: ");
        for (int i = 0; i < colorNum; i++) {
            init_pair(i+1, i, 0);
            attron(COLOR_PAIR(i + 1));
            if (i < 26) {
                mvprintw(centerY - 2 + i, centerX - (colorNum > 25 ? 15 : 10), "%d. %s", i, colorList[i]);
            } else {
                mvprintw(centerY - 2 + i - 25, centerX - 3, "%d. %s", i, colorList[i]);
            }
            attroff(COLOR_PAIR(i + 1));
        }
        mvprintw(centerY - 2, centerX - (colorNum > 25 ? 15 : 10), "0. Rainbow");
        refresh();
        do {
            choice = tolower(getchar());
        } while (!isdigit(choice) && choice != 'x');
        if (choice == 'x') optionMenu();
        int nextPick = choice - '0';
        do {
            choice = tolower(getchar());
        } while (!isdigit(choice) && choice != '\r');
        if (choice != '\r') nextPick = nextPick * 10 + choice - '0';
        if (nextPick == 0) nextPick = 99;
        nextPick++;
        for (int i = 0; i < colorNum; i++) init_pair(i+1, i, i);
        if (choice != 'x') {
            if (picked == 1) {
                SNAKE_COLOR = nextPick;
            } else if (picked == 2) {
                WALL_COLOR = nextPick;
            } else {
                APPLE_COLOR = nextPick;
            }
        } else optionMenu();
    } else {
        optionMenu();
    }
}

void getProfiles() {
    FILE *profiles;
    if (profiles = fopen("profiles.txt", "r")) {
        do {
            strIndex++;
            fscanf(profiles, "%s", &profList[strIndex]);
        } while (strlen(profList[strIndex]) != 0 && strIndex < 100);
        strIndex--;
        fclose(profiles);
    }
}

void updateScores() {
    char colrs[3], highs[6], prof[16], delay[5];
    if (Login == -1) {
        mvprintw(centerY, centerX - 14, "Do you want to save your score?");
        mvprintw(centerY + 1, centerX - 2, "Y/N");
        refresh();
        char cont;
        do {
            cont = tolower(getchar());
        } while(cont != 'y' && cont != 'n');
        if (cont == 'n') {
            Login = -2;
            return;
        }
        clear();
        mvprintw(centerY, centerX - 4, "Input your name: ");
        refresh();
        int cnt = 0;
        char newName[3] = "___";
        do {
            mvprintw(centerY + 2, centerX - 1, "%s", newName);
            refresh();
            char inp = toupper(getchar());
            if (isalpha(inp)) {
                if (inp == '-') {
                    newName[cnt] = '_';
                    cnt = ((cnt == 0) ? 0 : cnt - 1);
                } else {
                    newName[cnt] = inp;
                    cnt++;
                }
            }
        } while (cnt < 3);
        mvprintw(centerY + 2, centerX - 1, "%s", newName);
        refresh();
        getchar();
        strncpy(prof, newName, 4);
        prof[3] = '\0';
        Login = ++strIndex;
    } else {
        strncpy(prof, profList[Login], 3);
        prof[3] = '\0';
    }
    if (Login == -2) return;
    Colors = (SNAKE_COLOR > 99 ? 0 : SNAKE_COLOR) * 1e4 + (WALL_COLOR > 99 ? 0 : WALL_COLOR) * 1e2 + (APPLE_COLOR > 99 ? 0 : APPLE_COLOR);
    sprintf(colrs, "%d", Colors);
    sprintf(highs, "%d", highScore / 100);
    sprintf(delay, "%d", DELAY);
    strcat(prof, colrs);
    strcat(prof, ",");
    strcat(prof, delay);
    strcat(prof, ",");
    strcat(prof, highs);
    strcpy(profList[Login], prof);
}

void giveProfiles() {
    updateScores();
    FILE *profiles;
    profiles = fopen("profiles.txt", "w");
    for (int i = 0; i < strIndex + 1; i++) fprintf(profiles, "%s\n", profList[i]);
    fclose(profiles);
}

int login() {
    clear();
    mvprintw(centerY, centerX - 10, "Do you want to login? y/n");
    firstTime = true;
    refresh();
    char login = tolower(getchar());
    if (login == 'y') {
        Login = 0;
        clear();
        for (int i = 0; i < strIndex + 1; i++) ((strIndex + 1) < 50) ? mvprintw(centerY - strIndex / 2 + i, centerX - 3, "%d: %.3s", i + 1, profList[i]) : mvprintw(centerY - strIndex / 4 + (i - ((i >= (strIndex+1) / 2) ? (strIndex+1) / 2 : 0)), centerX - 5 + ((i >= (strIndex + 1) / 2) ? 10 : 0), "%d: %.3s", i + 1, profList[i]);
        refresh();
        bool isDub = false, doubleDig = false;
        if (strIndex > 9) isDub = true;
        do {
            do {
                do {
                    login = tolower(getchar());
                } while(!isdigit(login));
                Login += (login - '0') * (doubleDig ? 10 : 1);
                doubleDig = true;
            } while (login != '\r' && !(doubleDig || isDub));
        } while ((Login < 0) && (Login > strIndex + 2));
        Login--;
        char *next = profList[Login] + 3;
        char *last;
        Colors = strtol(next, &last, 10);
        next = last + 1;
        DELAY = strtol(next, &last, 10);
        next = last + 1;
        highScore = strtol(next, &last, 10) * 1e2;
        SNAKE_COLOR = Colors / 1e4;
        WALL_COLOR = (Colors - SNAKE_COLOR * 1e4) / 1e2;
        APPLE_COLOR = Colors - SNAKE_COLOR * 1e4 - WALL_COLOR * 1e2;
        if (SNAKE_COLOR == 0) SNAKE_COLOR = 100;
        if (WALL_COLOR == 0) WALL_COLOR = 100;
        if (APPLE_COLOR == 0) APPLE_COLOR = 100;
        return Login;
    } else {
        return -1;
    }
}

int snakePos(char dir) {
    bool axis;
    int intDir;
    switch (tolower(dir)) {
        case 'a':
            intDir = -1;
            axis = true;
            break;
        case 'd':
            intDir = 1;
            axis = true;
            break;
        case 's':
            intDir = 1;
            axis = false;
            break;
        case 'w':
            intDir = -1;
            axis = false;
            break;
        default:
            break;
    }
    snakeArr[0].prevX = snakeArr[0].x;
    snakeArr[0].prevY = snakeArr[0].y;
    for (int i = 1; i < snakeLen + 1; i++) {
            snakeArr[i].prevX = snakeArr[i].x;
            snakeArr[i].prevY = snakeArr[i].y;
            snakeArr[i].x = snakeArr[i - 1].prevX;
            snakeArr[i].y = snakeArr[i - 1].prevY;
    }
    int nextX = snakeArr[0].x + (axis ? intDir : 0);
    int nextY = snakeArr[0].y + (!axis ? intDir : 0);
    if (nextX == applePos.x && nextY == applePos.y) {
        snakeLen++;
        newApple();
    }
    bool fireDead = false;
    if (fireMode) {
        for (int i = 0; i < fireNum; i++) {
            if (nextX == fireArr[i].x && nextY == fireArr[i].y) fireDead = true; 
        }
    }
    for (int i = 0; i < snakeLen; i++)  {
        if (fireDead || ((snakeArr[i].x == nextX && snakeArr[i].y == nextY) || HEIGHT == nextY || WIDTH == nextX || nextX == 0 || nextY == 0)) {
            updateScores();
            clear();
            firstTime = true;
            score = fireMode ? score : snakeLen * 100;
            if (score > highScore) {
                highScore = score; 
                if (score > leaderBoard[0]) {
                    leaderBoard[0] = snakeLen * 1e2;
                    mvprintw(centerY, MAX_X  / 2- 15, "You got the HIGH SCORE of %d", highScore);
                } else
                mvprintw(centerY, MAX_X  / 2 - 21, "You got a personal best, but less then %d", leaderBoard[0]);
            } else {
                mvprintw(centerY, centerX - 21, "You got less then the HIGH SCORE of %d :(", leaderBoard[0]);
            }
            mvprintw(centerY + 2, centerX - 11, "Press enter to continue");
            refresh();
            char playAgain;
            do {
                playAgain = tolower(getchar());
            } while (playAgain != '\r' && playAgain != 'x');
            clear();
            if (playAgain == 'x') return 1;
            start = true;
            if (!fireMode) snakeLen = 1;
            score = 0;
            nextX = WIDTH / 2;
            nextY = HEIGHT / 2;
            if (fireMode) {
                fireNum = 0;
                newFire();
            } else {
                newApple();
            }
            break;
        }
        if (start) break;
        if (snakeArr[i].x == applePos.x && snakeArr[i].y == applePos.y) while (snakeArr[i].x == applePos.x && snakeArr[i].y == applePos.y) { 
            newApple();
        }
    }
    snakeArr[0].x = nextX;
    snakeArr[0].y = nextY;
    return 0;
}

char snakeMove(char oldDir) {
    char dir = ' ';
    if (start) {
        dir = tolower(getchar());
        start = false;
    } else {
        dir = tolower(getch());
    }
    if (snakeLen != 1) {
        switch (oldDir) {
            case 'a':
                if (dir == 'd') dir = 'a';
                break;
            case 'd':
                if (dir == 'a') dir = 'd';
                break;
            case 's':
                if (dir == 'w') dir = 's';
                break;
            case 'w':
                if (dir == 's') dir = 'w';
                break;
        }
    }
    return dir;
}

void printSnake() {
    if (firstTime || WALL_COLOR == 100) {
        for (int i = 0; i < WIDTH; i++) {
            colorChange(0, WALL_COLOR, i, NULL,0,0);
        }
        firstTime = false;
    }
    if (SNAKE_COLOR == 100) {
        for (int i = 0; i < snakeLen; i++) {
            colorChange(2, SNAKE_COLOR, i, NULL, snakeArr[i].x, snakeArr[i].y);
        }
        mvprintw(centerY - HEIGHT / 2 + snakeArr[snakeLen].y, centerX - WIDTH / 2 + snakeArr[snakeLen].x, " ");
    } else {
        init_pair(SNAKE_COLOR, SNAKE_COLOR - 1, SNAKE_COLOR - 1);
        attron(COLOR_PAIR(SNAKE_COLOR));
        mvprintw(centerY - HEIGHT / 2 + snakeArr[0].y, centerX - WIDTH / 2 + snakeArr[0].x, "X");
        attroff(COLOR_PAIR(SNAKE_COLOR));
        mvprintw(centerY - HEIGHT / 2 + snakeArr[snakeLen - 1].prevY, centerX - WIDTH / 2 + snakeArr[snakeLen - 1].prevX, " ");
    }
    if (!fireMode) {
        colorChange(1, APPLE_COLOR, 1, NULL, applePos.x, applePos.y);
    } else {
        for (int i = 0; i < fireNum; i++) {
            colorChange(1,3,i,NULL, fireArr[i].x, fireArr[i].y);
        }
    }
    mvprintw(centerY + HEIGHT / 2 + 2, centerX - WIDTH / 2, "Score: %d  ", fireMode ? score : snakeLen * 100);
    mvprintw(centerY + HEIGHT / 2 + 2, centerX + WIDTH / 2 - 18, "Personal Best: %d", highScore);
    mvprintw(centerY + HEIGHT / 2 + 4, centerX - 8, "Highscore: %d", leaderBoard[0]);
    refresh();
}

int main() {
    snakeArr[0].x = WIDTH / 2;
    snakeArr[0].y = HEIGHT / 2;
    snakeArr[0].prevX = WIDTH / 2;
    snakeArr[0].prevY = HEIGHT / 2;
    setenv("TERM", "xterm-256color", 1);
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    getmaxyx(stdscr, MAX_Y, MAX_X);
    getTime(0.0, 0);
    centerX = MAX_X / 2;
    centerY = MAX_Y / 2;
    start_color();
    getColors();
    srand(time(NULL));
    getProfiles();
    if (strIndex != -1) Login = login();
    newApple();
    while (true) {
        int startOp = startMenu();
        if (startOp == 1) {
            clear();
            mvprintw(MAX_Y / 2, MAX_X / 2 - 10, "Select a gamemode: ");
            mvprintw(MAX_Y / 2 + 1, MAX_X / 2 - 10, "1. Classic");
            mvprintw(MAX_Y / 2 + 2, MAX_X / 2 - 10, "2. Wildfire");
            firstTime = true;
            refresh();
            char gameSelection;
            do {
                gameSelection = getchar();
            } while (!isdigit(gameSelection));
            int picked = gameSelection - '0';
            if ( picked == 1) {
                fireMode = false;
            }
            else if (picked == 2) {
                fireMode = true;
                snakeLen = 3;
                newFire();
            }
            char oldDir;
            clear();
            while (true) {
                if (fireMode && getTime(5, 4)) {
                    score += 100;
                    newFire();
                }
                refresh();
                int check = 0;
                printSnake();
                char DIR = snakeMove(oldDir);
                if (DIR == 'x' || DIR == ' ') {
                    if (DIR == ' ') {
                        mvprintw(centerY, centerX - 3, "Paused");
                        mvprintw(centerY + 1, centerX - 17, "Press enter to resume, press X to quit");
                        refresh();
                        do {
                            DIR = tolower(getchar());
                        } while (DIR != '\r' && DIR != 'x');
                    }
                    if (DIR == 'x') {
                        giveProfiles();
                        break;
                    }
                } else if (strchr("wasd", DIR) != NULL) {
                    check = snakePos(DIR);
                    oldDir = DIR;
                } else {
                    check = snakePos(oldDir);
                }
                if (check) {
                    giveProfiles();
                    break;
                }
                while (!getTime((double)DELAY / 1e3, 3));
            }
        } else if (startOp == 2) {
            optionMenu();
        } else if (startOp == 4) {
            giveProfiles();
            endwin();
            use_default_colors();
            exit(-1);
        } else if (startOp == 3) Login = login();
    }
}