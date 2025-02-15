#include "../include/ctring.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MATCH 2
#define MISMATCH -1
#define GAP -2

typedef struct {
    int **matrix;
    int **tracebackMatrix;
    size_t row_size, col_size;
    char *row_text, *col_text;
    int max_score;
    int score_x;
    int score_y;
    char *tracebackStr;
} Matrix;

enum trace { enumMatch, enumGap1, enumGap2 };

// typedef struct {
//     int val;
//
// }

int **zeroMatrix(size_t rows, size_t cols) {
    int **m = malloc(rows * sizeof(int *));
    for (size_t i = 0; i < rows; i++) {
        m[i] = malloc(cols * sizeof(int));
        for (size_t j = 0; j < cols; j++) {
            m[i][j] = 0; // Initialize to zero
        }
    }
    return m;
}

Matrix *newMatrix(char *row, char *col) {
    Matrix *m = malloc(sizeof(Matrix));

    m->row_text = row;
    m->row_size = strlen(row) + 1;
    m->col_text = col;
    m->col_size = strlen(col) + 1;

    m->matrix = zeroMatrix(m->row_size, m->col_size);
    m->tracebackMatrix = zeroMatrix(m->row_size, m->col_size);

    return m;
}

void printMatrix(Matrix *m) {
    for (size_t i = 0; i < m->row_size; i++) {
        for (size_t j = 0; j < m->col_size; j++) {
            printf("%2d ", m->matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
    for (size_t i = 0; i < m->row_size; i++) {
        for (size_t j = 0; j < m->col_size; j++) {
            printf("%2d ", m->tracebackMatrix[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}

int max(int a, int b, int c) {
    int max = (a > b && a > c) ? a : ((b > c) ? b : c);
    return max > 0 ? max : 0;
}

void traceback(Matrix *m) {
    Ctring *str = ctring("");
    int i = m->score_x;
    int j = m->score_y;
    while (m->matrix[i][j] != 0) {
        if (m->tracebackMatrix[i][j] == enumMatch) {
            char *s = malloc(2 * sizeof(char));
            s[0] = m->row_text[i - 1];
            s[1] = '\0';
            prepend(str, s);
            i--;
            j--;
        } else if (m->tracebackMatrix[i][j] == enumGap1) {
            i--;
        } else {
            j--;
        }
    }
    m->tracebackStr = str->literal;
}

int smith_waterman(Matrix *m) {
    int maxScore = 0;
    for (size_t i = 1; i < m->row_size; i++) {
        for (size_t j = 1; j < m->col_size; j++) {
            int match =
                m->matrix[i - 1][j - 1] +
                (m->row_text[i - 1] == m->col_text[j - 1] ? MATCH : MISMATCH);
            int gap1 = m->matrix[i - 1][j] + GAP;
            int gap2 = m->matrix[i][j - 1] + GAP;
            int maxVal = max(match, gap1, gap2);
            m->tracebackMatrix[i][j] = (maxVal == match)  ? enumMatch
                                       : (maxVal == gap1) ? enumGap1
                                                          : enumGap2;
            m->matrix[i][j] = maxVal;
            if (maxVal > maxScore) {
                maxScore = maxVal;
                m->score_x = i;
                m->score_y = j;
            }
        }
    }
    m->max_score = maxScore;
    return maxScore;
}

Matrix *computeMatrix(char *s1, char *s2) {
    Matrix *m = newMatrix(s1, s2); // FIXME: free(Matrix);
    smith_waterman(m);
    traceback(m);
    return m;
}

int main() {
    char *s1 = "darshpsps";
    char *s2 = "afrshpsps";

    Matrix *m = computeMatrix(s1, s2);

    printMatrix(m);
    printf("%d\n", m->max_score);
    printf("%s\n", m->tracebackStr);

    return 0;
}
