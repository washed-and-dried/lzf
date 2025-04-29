#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    struct dirent *de; // DIRectory ENtry
    DIR *dir = opendir(".");
    if (dir == NULL) {
        printf("failed opening directory");
        exit(1);
    }

    initscr();

    printw("\n");
    printw("\n");
    while ((de = readdir(dir)) != NULL) { //TODO: maybe write a dynamic arrays library (like ctring)
        if (de->d_type != DT_DIR) {
            printw("%s\n", de->d_name);
        }
        refresh();
    }
    closedir(dir);

    getch();
    endwin();

    return 0;
}
