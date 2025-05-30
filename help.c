#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // 追加

void show_help() {
    clear();
    FILE* fp = fopen("help.txt", "r");
    if (!fp) {
        fprintf(stderr, "ヘルプを表示できません\n");
        return;
    }
    char line[256];
    int row = 2;
    while (fgets(line, sizeof(line), fp) != NULL) {
        // 改行文字を除去
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        mvprintw(row++, 2, "%s", line);
    }
    fclose(fp);
    mvprintw(row + 1, 2, "任意のキーを押してメニューに戻る...");
    refresh();
    getch();
}
