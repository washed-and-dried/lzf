#include <algorithm>
#include <cstdio>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include "smith_waterman.c"

using namespace std;

// typedef struct {
//     char** data; // its basically a pointer to an array of strings
//     size_t len;
//     size_t capacity;
// } DA;
//
// DA* MAKE_DA() {
//     DA* da = malloc(sizeof(DA) * 1); // allocate on heap (mf C)
//     da->capacity = 10;
//     da->len = 0;
//     da->data = malloc(sizeof(char*) * da->capacity);
//
//     return da;
// }
//
// void DA_APPEND(DA* da, char* str) {
//     if (da->len == da->capacity) {
//         da->capacity *= 2;
//         char** data = malloc(sizeof(char*) * da->capacity);
//         memmove(data, da->data, da->len);
//     }
//
//     da->data[da->len++] = str;
// }

int main(int argc, char** argv) {
    if (argc != 3) { // FIXME: hard coded for now to only accept the search string and directory
        printf("lzf [SEARCH STRING] [DIR]\n");

        exit(EXIT_FAILURE);
    }

    char* search_string = argv[1];
    char* directory = argv[2];

    struct dirent *de; // DIRectory ENtry
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        printf("failed opening directory");
        exit(EXIT_FAILURE);
    }

    vector<pair<float, int>> sorts; // FIXME: maybe replace with min or max heap?
    vector<char*> files; // <string> does not work dwag. I don't know this shit :cry

    // FIXME: handle directories and make it recursive descent
    while ((de = readdir(dir)) != NULL) { //TODO: maybe write a dynamic arrays library (like ctring)
        if (de->d_type != DT_DIR) {
            float score = compare_string(search_string, de->d_name);
            printf("%s -> %f\n", de->d_name, score);
            files.push_back(de->d_name);
            sorts.push_back({score, (int)files.size() - 1});
        } else {
            printf("OTHER TYPE: [%d : %s]\n", de->d_type, de->d_name);
        }
    }

    sort(sorts.begin(), sorts.end(), [&](auto& a, auto& b){
        return a.first > b.first;
    });

    for (const auto& [score, idx]: sorts) {
        printf("%s\n", files[idx]);
    }
}
