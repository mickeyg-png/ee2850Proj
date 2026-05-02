#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <stdbool.h>

bool func(char *text) {
    return (text == NULL);
}

int main() {
    char balls[10] = "b";
    printf("%d", func());
    return 0;
}