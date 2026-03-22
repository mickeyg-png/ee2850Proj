#include <stdio.h>
#include <string.h>
#include <ncurses.h>
int main() {
    for (int i = 0; i < 5; i++) {
        char balls[3];
        sprintf(balls, "f%d", i);
        printf("%s\n", balls);
    }
}