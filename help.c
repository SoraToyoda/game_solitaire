#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

void show_help() {
    clear();
    FILE* fp = fopen("help.txt", "r");
    if (!fp) {
        fprintf(stderr, "ヘルプを表示できません\n");
        return;
    }
    char line[256];
    int row = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        mvprintw(row++, 0, "%s", line);
    }
    fclose(fp);
    mvprintw(row + 1, 0, "任意のキーを押してメニューに戻る...");
    refresh();
    getch();
}