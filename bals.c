#include <stdio.h>
#include <string.h>
#include <ncurses.h>
int main() {
    int balsl = can_change_color();
    printf("%d", balsl);
}