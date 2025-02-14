#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MATCH 2
#define MISMATCH -1
#define GAP -2

typedef struct {
    int **matrix;
    size_t row_size, col_size;
    char *row_text, *col_text;
} Matrix;

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
}

int max(int a, int b, int c) {
    int max = (a > b && a > c) ? a : ((b > c) ? b : c);
    return max > 0 ? max : 0;
}

void smith_waterman(Matrix *m) {
    for (size_t i = 1; i < m->row_size; i++) {
        for (size_t j = 1; j < m->col_size; j++) {
            int match =
                m->matrix[i - 1][j - 1] +
                (m->row_text[i - 1] == m->col_text[j - 1] ? MATCH : MISMATCH);
            int gap1 = m->matrix[i - 1][j] + GAP;
            int gap2 = m->matrix[i][j - 1] + GAP;
            m->matrix[i][j] = max(match, gap1, gap2);
        }
    }
}

int main() {
    char *s1 = "GGATCGA";
    char *s2 = "GAATTCAGTTA";

    Matrix *m = newMatrix(s1, s2);

    smith_waterman(m);
    printMatrix(m);

    return 0;
}
