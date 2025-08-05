#include "smith_waterman.c"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <string>
#include <sys/types.h>
#include <vector>

// for the TUI
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>

using namespace std;

#define MAX_LEVEL 2
#define MAX_ENTRIES 50
#define SEPARATOR_CHAR L'' // FIXME: not used currently, fix it dawg

#define QUIT CTRL('d')
#define ESCAPE CTRL('[')
#define KEY_BACKSPACE 127

#define MOVE_DOWN(LINE) printf("\E[%dB", LINE);
#define MOVE_DOWN_START(LINE) printf("\E[%dE", LINE);

#define MOVE_UP(LINE) printf("\E[%dA", LINE);
#define MOVE_UP_START(LINE) printf("\E[%dF", LINE);

int height = 0;
int width = 0;

void recurse_fs(char *dirname, vector<pair<float, int>> &sorts,
                vector<string> &files, char *search_string, int level) {
    struct dirent *de; // DIRectory ENtry

    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        // printf("failed opening directory: %s\n", dirname); // FIXME: maybe
        // don't fail just print warning and return? (like permission denied)
        // exit(EXIT_FAILURE);
        closedir(dir);
        return;
    }

    while ((de = readdir(dir)) != NULL) {
        char *path = (char *)malloc(sizeof(char) *
                                    (strlen(dirname) + strlen(de->d_name) + 2));
        strcpy(path, dirname);

        if (strcmp(dirname, "/")) { // dirname is not a /
            strcat(path, "/");
        }

        strcat(path, de->d_name);

        if (de->d_type != DT_DIR) {
            float score = compare_string(search_string, de->d_name);

            // printf("%s -> %f\n", de->d_name, score);

            files.push_back(path);
            sorts.push_back({score, (int)files.size() - 1});
        } else { // if its a dir, recurse into it dawg
            if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) {
                free(path);
                continue;
            }
            if (!strcmp(de->d_name, ".git") || !strcmp(de->d_name, ".github") ||
                !strcmp(de->d_name, ".cache")) {
                free(path);
                continue;
            }

            if (level < MAX_LEVEL) { // only recurse upto set max level
                recurse_fs(path, sorts, files, search_string, level + 1);
            }
        }

        free(path);
    }

    closedir(dir);
}

// we must always revert back to orignal terminal state after we are finished
struct termios orig_termios;

void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);

    atexit(disableRawMode);

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void process_files(char *prompt, int prompt_t, char **argv) {
    vector<pair<float, int>> sorts;
    vector<string> files;

    // FIXME: replace with ctring when we get character append
    char *search_string = (char *)malloc(sizeof(char) * prompt_t);
    memcpy(search_string, prompt, prompt_t);

    char *directory = argv[2];

    recurse_fs(directory, sorts, files, search_string, 0);

    const int MAX_FIT =
        (height - 3) >= MAX_ENTRIES ? MAX_ENTRIES : (height - 3);

    printf("%d / %zu entries\n", MAX_FIT, sorts.size());

    sort(sorts.begin(), sorts.end(),
         [&](auto &a, auto &b) { return a.first > b.first; });

    for (int i = 0; i < MAX_FIT && i < files.size() && i < sorts.size(); i++) {
        float score = sorts[i].first;
        int idx = sorts[i].second;

        if (score == 0.0)
            continue; // no need to print useless entries

        if (i != 0) { // don't ask me why dawg. It is what it is
            printf("\n");
        }
        printf("%2d. %s -> %f", i, files[idx].c_str(), score);
    }
}

void resize(int _) {
    struct winsize ws;
    ioctl(1, TIOCGWINSZ, &ws);
    height = ws.ws_row;
    width = ws.ws_col;
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "");
    signal(SIGWINCH, resize);
    resize(0);

    if (argc != 3) { // FIXME: hard coded for now to only accept the search
                     // string and directory
        printf("lzf [SEARCH STRING] [DIR]\n");

        exit(EXIT_FAILURE);
    }

    setvbuf(stdout, NULL, _IONBF, 0);
    enableRawMode(); // disabled on callback

    char prompt[256];
    int prompt_t =
        0; // NOTE: FUCKING LONG IS FUCKED, SIZE_T IS LONG UNSIGNED INT AND LONG
           // IS FUCKED SO CANT USE SIZE_T OR LONG IN CURRENT SCOPE
    int ch;

    printf("\E[H\E[2J"); // clear screen

    // print the separator
    MOVE_DOWN_START(1);
    for (int i = 0; i < width; i++) {
        printf("%lc", SEPARATOR_CHAR);
    }
    MOVE_UP_START(1);

    while (read(STDIN_FILENO, &ch, 1) == 1) {
        switch (ch) {
        case ESCAPE:
        case QUIT: {
            printf("\n");
            exit(0);
        } break;
        case CTRL('h'):
        case KEY_BACKSPACE: {
            if (prompt_t <= 0)
                break;

            prompt_t--;

            printf("\E[1D \E[1D");
        } break;
        case CTRL('l'): {  // clear screen
            printf("\E7"); // save cursor
            MOVE_DOWN_START(2)
            printf("\E[0J"); // erase old content
            printf("\E8");   // restore cursor
        } break;

        case CTRL('u'): { // clear input
            printf("\E[1J");
            printf("\E[H"); // move cursor to home position
            prompt_t = 0;
        } break;

        case '\n':         // ENTER
        case '\r': {       // \n and \r since its byte-by-byte input in raw mode
            printf("\E7"); // save cursor
            // first we move two lines down:- one for input and one for
            // separator
            MOVE_DOWN_START(2)
            printf("\E[0J");   // erase old content
            printf("\E[?25l"); // invisible cursor

            prompt[prompt_t] = '\0';

            process_files(prompt, prompt_t, argv);

            printf("\E[?25h"); // visible cursor
            printf("\E8");     // restore cursor
        } break;
        default: {
            if (!isprint(ch))
                break;

            if (prompt_t >= 256) {
                printf("%s\n", "Reached max character limit of 1024 chars for "
                               "shell promt");
                exit(130);
            }

            prompt[prompt_t++] = ch;

            printf("%c", ch);
            break;
        }
        }
    }
}
