#include <algorithm>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <string>
#include "smith_waterman.c"

using namespace std;

#define MAX_LEVEL 2
#define MAX_ENTRIES 50

void recurse_fs(char* dirname, vector<pair<float, int>>& sorts, vector<string>& files, char* search_string, int level) {
    struct dirent *de; // DIRectory ENtry

    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        printf("failed opening directory: %s\n", dirname); // FIXME: maybe don't fail just print warning and return? (like permission denied)
        // exit(EXIT_FAILURE);
        free(dir);
        return;
    }

    while ((de = readdir(dir)) != NULL) { //TODO: maybe write a dynamic arrays library (like ctring)
        char* path = (char*) malloc(sizeof(char) * (strlen(dirname) + strlen(de->d_name) + 2));
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
            if (!strcmp(de->d_name, ".git") || !strcmp(de->d_name, ".github") || !strcmp(de->d_name, ".cache")) {
                free(path);
                continue;
            }

            // printf("OTHER TYPE: [%d : %s]\n", de->d_type, de->d_name);

            // FIXME: THIS LEVEL THING IS BEHAVING UNDEFINED. GOTTA CHECK WHAT'S UP WITH THIS
            if (level < MAX_LEVEL) { //FIXME:: for now limit depth to MAX_LEVEL
                // printf("NEW PATH: %s\n", path);

                recurse_fs(path, sorts, files, search_string, level + 1);
            }
        }

        free(path);
    }

    free(dir);
    free(de);
}

int main(int argc, char** argv) {
    if (argc != 3) { // FIXME: hard coded for now to only accept the search string and directory
        printf("lzf [SEARCH STRING] [DIR]\n");

        exit(EXIT_FAILURE);
    }

    char* search_string = argv[1];
    char* directory = argv[2];

    vector<pair<float, int>> sorts;
    vector<string> files;

    recurse_fs(directory, sorts, files, search_string, 0);

    sort(sorts.begin(), sorts.end(), [&](auto& a, auto& b){
        return a.first > b.first;
    });

    // print only first 50 entires
    for (int i = 0; i < MAX_ENTRIES && i < files.size() && i < sorts.size(); i++) {
        float score = sorts[i].first;
        int idx = sorts[i].second;

        if (score == 0.0) continue; // no need to print useless entries

        printf("%2d. %s -> %f\n", i, files[idx].c_str(), score);
    }
}
