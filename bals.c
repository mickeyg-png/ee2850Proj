#include <stdio.h>
#include <string.h>
#include <ncurses.h>
int main() {
    char balls = getchar();
    printf("%d", balls == '\n');
    return 0;
}