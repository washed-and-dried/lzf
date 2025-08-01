#include "../include/ctring.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

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

int **zeroMatrix(size_t rows, size_t cols) {
    int **m = (int**) malloc(rows * sizeof(int *));
    for (size_t i = 0; i < rows; i++) {
        m[i] = (int*) malloc(cols * sizeof(int));
        for (size_t j = 0; j < cols; j++) {
            m[i][j] = 0; // Initialize to zero
        }
    }
    return m;
}

Matrix *newMatrix(char *row, char *col) {
    Matrix *m = (Matrix*) malloc(sizeof(Matrix));

    m->row_text = row;
    m->row_size = strlen(row) + 1;
    m->col_text = col;
    m->col_size = strlen(col) + 1;
    m->score_x = 0;
    m->score_y = 0;

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
            char *s = (char*) malloc(2 * sizeof(char));
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

double normalized_score_fast(Matrix *m, char *s1, char *s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    size_t min_len = len1 < len2 ? len1 : len2;

    int best_possible_score = min_len * MATCH;

    if (best_possible_score == 0) return 0.0;

    return (double)m->max_score / best_possible_score;
}

float compare_string(char* s1, char* s2) {
    Matrix *m = computeMatrix(s1, s2);

    //  score must be normalized to compare string. Otherwise it may introduce length bias for local alignment
    float norm_score = normalized_score_fast(m, s1, s2);

#if 0
    printMatrix(m);
    printf("%d\n", m->max_score);
    printf("%s\n", m->tracebackStr);
    printf("%f\n", norm_score);
#endif

    return norm_score;
}
