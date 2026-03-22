#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
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
clock_t start_t, end_t;

struct snake {
    int x, y, prevX, prevY;
    bool end;
};

struct snake snakeArr[MAX_LEN];

typedef struct {
    int x,y;
} apple;

apple applePos;

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

double colorCnt = 0;

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
    bool rainCheck = false;
    do {
        colorCnt += .01;
        if (SNAKE_COLOR > 99) {
            SNAKE_COLOR -= 99;
            rainCheck = true;
        }
        init_pair(SNAKE_COLOR, SNAKE_COLOR - 1, 0);
        init_pair(WALL_COLOR, WALL_COLOR - 1, 0);
        init_pair(APPLE_COLOR, APPLE_COLOR - 1, 0);
        attron(COLOR_PAIR(SNAKE_COLOR));
        mvprintw(centerY, centerX + 20, "Snake");
        attroff(COLOR_PAIR(SNAKE_COLOR));
        if (rainCheck) SNAKE_COLOR += (colorCnt > .15 ? (SNAKE_COLOR == 7 ? 94 : 100) : 99);
        rainCheck = false;
        if (WALL_COLOR > 99) {
            WALL_COLOR -= 99;
            rainCheck = true;
        }
        attron(COLOR_PAIR(WALL_COLOR));
        mvprintw(centerY + 1, centerX + 20, "Wall");
        attroff(COLOR_PAIR(WALL_COLOR));
        if (rainCheck) WALL_COLOR += (colorCnt > .15 ? (WALL_COLOR == 7 ? 94 : 100) : 99);
        rainCheck = false;
        if (APPLE_COLOR > 99) {
            APPLE_COLOR -= 99;
            rainCheck = true;
        }
        attron(COLOR_PAIR(APPLE_COLOR));
        mvprintw(centerY +2, centerX + 20, "Apple");
        attroff(COLOR_PAIR(APPLE_COLOR));
        if (rainCheck) APPLE_COLOR += (colorCnt > .15 ? (APPLE_COLOR == 7 ? 94 : 100) : 99);
        rainCheck = false;
        if (colorCnt > .15) colorCnt = 0;
        Sleep(50);
        choice = getch();
    } while (!isdigit(choice));
    int picked = choice - '0';
    init_pair(SNAKE_COLOR, SNAKE_COLOR - 1, SNAKE_COLOR - 1);
    init_pair(WALL_COLOR, WALL_COLOR - 1, WALL_COLOR - 1);
    init_pair(APPLE_COLOR, APPLE_COLOR - 1, APPLE_COLOR - 1);
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
        } else {
            colorInd = 8;
        }
    } else {colorInd = 8;
    }
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
    bool two = false, five = false;
    char inp;
    for (int i = 0; i < 3; i++) {
        char *garbage;
        int cnt = 0;
        char newCol[4] = "\0";
        do {
            mvprintw(centerY + 3, centerX - 5 + 4 * i, "%s", newCol);
            refresh();
            inp = getchar();
            if (inp == '-') {
                newCol[cnt] = '\0';
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
            scanf("%d", &DELAY);
            mvprintw(centerY + 1, centerX - 17, "Inputted: %d, Press enter to save", DELAY);
            mvprintw(centerY + 2, centerX - 9, "Press X to change");
            refresh();
            char choice; 
            do {
                choice = tolower(getchar());
            } while (choice != '\r' && choice != 'x');
            if (choice == ' ') break;
            DELAY = 50;
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
        for (int i = 0; i < colorNum + 1; i++) {
            init_pair(i+1, i, 0);
            attron(COLOR_PAIR(i + 1));
            if (i < 26) {
                mvprintw(centerY - 4 + i, centerX + 30, "%d. %s", i, colorList[i]);
            } else {
                mvprintw(centerY - 4 + i, centerX + 40, "%d. %s", i, colorList[i]);
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
        mvprintw(centerY - 3, centerX - 30, "Select color (For single digit end in a .), press X to return");
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
        } while (!isdigit(choice) && choice != '.');
        if (choice != '.') nextPick = nextPick * 10 + choice - '0';
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
    refresh();
    char login = tolower(getchar());
    if (login == 'y') {
        clear();
        for (int i = 0; i < strIndex + 1; i++) ((strIndex + 1) < 50) ? mvprintw(centerY - strIndex / 2 + i, centerX - 3, "%d: %.3s", i + 1, profList[i]) : mvprintw(centerY - strIndex / 4 + (i - ((i >= (strIndex+1) / 2) ? (strIndex+1) / 2 : 0)), centerX - 5 + ((i >= (strIndex + 1) / 2) ? 10 : 0), "%d: %.3s", i + 1, profList[i]);
        refresh();
        do {
            do {
                login = tolower(getchar());
            } while(!isdigit(login));
            Login = login - '0';
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

void snakePos(char dir) {
    bool axis;
    int intDir, pastXPos, pastYPos, doubleYPast, doubleXPast;
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
        snakeArr[snakeLen].end = false;
        snakeLen++;
        snakeArr[snakeLen].end = true;
        newApple();
    }
    for (int i = 0; i < snakeLen; i++)  {
        if ((snakeArr[i].x == nextX && snakeArr[i].y == nextY) || HEIGHT == nextY || WIDTH == nextX || nextX == 0 || nextY == 0) {
            updateScores();
            if (snakeLen * 100 > highScore) {
                highScore = snakeLen * 100; 
                if (snakeLen * 100 > leaderBoard[0]) {
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
            } while (playAgain != '\r');
            clear();
            start = true;
            snakeLen = 1;
            nextX = WIDTH / 2;
            nextY = HEIGHT / 2;
            newApple();
            break;
        }
        if (start) break;
        if (snakeArr[i].x == applePos.x && snakeArr[i].y == applePos.y) while (snakeArr[i].x == applePos.x && snakeArr[i].y == applePos.y) { 
            newApple();
        }
    }
    snakeArr[0].x = nextX;
    snakeArr[0].y = nextY;
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

int colorCh[3] = {0, 0, 0};

void printSnake() {
    bool snakeCheck = false, snakeDoubleCheck = false, appleCheck;
    int snakeBody;
    bool tick = false;
    end_t = clock();
    clock_t hold_t = start_t - end_t;
    if (hold_t > 2.5e5) {
        start_t = clock();
        tick = true;
    }
    for (int i = 0; i < WIDTH; i++) {
        bool balls = false;
        if (WALL_COLOR > 99) {
            balls = true;
            colorCh[0] += tick;
            WALL_COLOR = colorCh[0] % 6 + 2;
        }
        attron(COLOR_PAIR(WALL_COLOR));
        mvprintw(centerY - HEIGHT / 2, i + centerX - WIDTH / 2, "#");
        mvprintw(centerY + HEIGHT / 2 + 1, i + centerX - WIDTH / 2, "#");
        if (i <= HEIGHT) {
            mvprintw(centerY - HEIGHT / 2 + i, centerX - WIDTH / 2, "#");
            mvprintw(centerY - HEIGHT / 2 + i, centerX + WIDTH / 2, "#");
        }
        attroff(COLOR_PAIR(WALL_COLOR));
    }
    if (SNAKE_COLOR > 99) {
        for (int i = 0; i < snakeLen; i++) {
            colorCh[2] += tick;
            SNAKE_COLOR = (i + colorCh[2]) % 6 + 2;
            attron(COLOR_PAIR(SNAKE_COLOR));
            mvprintw(centerY - HEIGHT / 2 + snakeArr[i].y, centerX - WIDTH / 2 + snakeArr[i].x, "X");
            attroff(COLOR_PAIR(SNAKE_COLOR));
            SNAKE_COLOR = 100;
            if (snakeArr[i].end) mvprintw(centerY - HEIGHT / 2 + snakeArr[i].prevY, centerX - WIDTH / 2 + snakeArr[i].prevX, " ");
        }
    } else {
        attron(COLOR_PAIR(SNAKE_COLOR));
        mvprintw(centerY - HEIGHT / 2 + snakeArr[0].y, centerX - WIDTH / 2 + snakeArr[0].x, "X");
        attroff(COLOR_PAIR(SNAKE_COLOR));
        mvprintw(centerY - HEIGHT / 2 + snakeArr[snakeLen - 1].prevY, centerX - WIDTH / 2 + snakeArr[snakeLen - 1].prevX, " ");
    }
    bool balls = false;
    if (APPLE_COLOR > 99) {
        balls = true;
        colorCh[1] += tick;
        APPLE_COLOR = colorCh[1] % 6 + 2;
    }
    attron(COLOR_PAIR(APPLE_COLOR));
    mvprintw(centerY - HEIGHT / 2 + applePos.y, centerX - WIDTH / 2 + applePos.x, "@");
    attroff(COLOR_PAIR(APPLE_COLOR));
    mvprintw(centerY + HEIGHT / 2 + 2, centerX - WIDTH / 2, "Score: %d  ", snakeLen * 100);
    mvprintw(centerY + HEIGHT / 2 + 2, centerX + WIDTH / 2 - 18, "Personal Best: %d", highScore);
    mvprintw(centerY + HEIGHT / 2 + 4, centerX - 8, "Highscore: %d", leaderBoard[0]);
    refresh();
}

int main() {
    snakeArr[0].x = WIDTH / 2;
    snakeArr[0].y = HEIGHT / 2;
    snakeArr[0].prevX = WIDTH / 2;
    snakeArr[0].prevY = HEIGHT / 2;
    snakeArr[0].end = true;    
    start_t = clock();
    initscr();
    cbreak();            
    noecho();
    nodelay(stdscr, TRUE);
    getmaxyx(stdscr, MAX_Y, MAX_X);
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
            char oldDir;
            while (true) {
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
                } else if (DIR == 'w' || DIR == 'a' || DIR == 's' || DIR == 'd') {
                    snakePos(DIR);
                    oldDir = DIR;
                } else {
                    snakePos(oldDir);
                }
                Sleep(DELAY);
            }
        } else if (startOp == 2) {
            optionMenu();
        } else if (startOp == 4) {
            giveProfiles();
            endwin();
            exit(-1);
        } else if (startOp == 3) Login = login();
    }
}