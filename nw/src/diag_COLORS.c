#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "rdtsc.h"
#include <omp.h>

#include "gene_bin.h"
#include "gene_bin.c"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

//////////////// Print Similarity Matrix
/**
 * Prints the similarity matrix
 *
 * in : F : The similarity matrix in COL MAJOR. Its dimensions are `m*n`.
 * in : A : Horizontal char array. Its size is `m-1`
 * in : B : Vertical char array. Its size is `n-1`
 * in : m : Number of columns of `F`. Also the `A` length + 1
 * in : n : Number of lines of `F`. Also the `B` length + 1
 * in : k : Colors background of selected case in `F`, which position is `k` and `l`. Defaults it to 0 if not intended to use.
 * in : l : Colors background of selected case in `F`, which position is `k` and `l`. Defaults it to 0 if not intended to use.
 */
void print_sim_mat(int* F, int* Fcolors, char A [], char B [], int m, int n, int k, int l) {
    int couleur = 81;
    printf("%2s %2s ", "*", "-");
    for (int j = 0; j < m; j++)
        printf("%2c ", A[j]);
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("%2c ", i == 0 ? '-' : B[i - 1]);
        for (int j = 0; j < m; j++) {
            if (i == k && j == l)
                printf("\033[101m% 2d \033[0m", F[i * m + j]);
            else if (i >= 1 && j >= 1) {
                printf("\033[29m\033[1m\033[48;5;%dm% 2d \033[0m", Fcolors[i * m + j], F[i * m + j]);
                couleur--;
            }
            else
                printf("% 2d ", F[i * m + j]);
        }
        printf("\n");
    }
}


//////////////// String Insert
/**
 * Insert a char at beginning of a char array.
 *
 * in/out : dest   : Destination string, to which we insert the `source` char
 * in     : source : Char to be inerted at beginning of `dest`.
 */
void string_insert(char** dest, char* source) {
    if (*dest == NULL) {
        *dest = calloc(sizeof(char), 2);
        memcpy(*dest, source, 1);
        return;
    }
    char* temp = calloc(sizeof(char), strlen(*dest) + 2);
    memcpy(temp, source, 1);
    strcat(temp, *dest);
    free(*dest);
    *dest = temp;
}

//////////////// Align
/**
 * Backtrace the similarity matrix, and prints the two sequences aligned.
 * Returns the max similarity score of the two sequences.
 *
 * in  : F : The similarity matrix in COL MAJOR. Its dimensions are given by (`A` length + 1)*(`B` length + 1).
 * in  : A : Horizontal char array.
 * in  : B : Vertical char array.
 * in  : match : Score for a match.
 * in  : mismatch : Score for a mismatch.
 * in  : gap : Score for a gap.
 * in  : print : Enables (1) or Disables (0) alignment output. Returns max similarity score either ways.
 * out : Max similarity score of the `A` and `B` sequences.
 */
int align(int* F, char A [], char B [], int match, int mismatch, int gap, int print) {
    int m = strlen(A) + 1;
    int n = strlen(B) + 1;

    if (print) {
        int i = m - 1;
        int j = n - 1;

        char* A_aligned = NULL;
        char* B_aligned = NULL;

        while (i > 0 || j > 0) {
            if (i > 0 && j > 0 && F[i * m + j] == F[(i - 1) * m + j - 1] + (A[i - 1] == B[j - 1] ? match : mismatch)) {
                string_insert(&A_aligned, A + (i - 1));
                string_insert(&B_aligned, B + (j - 1));
                i--;
                j--;
            }
            else if (i > 0 && F[i * m + j] == F[(i - 1) * m + j] + gap) {
                string_insert(&A_aligned, A + (i - 1));
                string_insert(&B_aligned, "-");
                i--;
            }
            else {
                // else if (j > 0 && F[i * m + j] == F[i * m + j - 1] + gap) {
                string_insert(&A_aligned, "-");
                string_insert(&B_aligned, B + (j - 1));
                j--;
            }
        }
        printf("%s\n", A_aligned);
        printf("%s\n", B_aligned);
    }
    return F[m * n - 1];
}

int* diag_calculate_scoring_matrix(char A [], char B [], int match, int mismatch, int gap) {

    int colors = 81;
    int m = strlen(A) + 1;
    int n = strlen(B) + 1;

    int* F = NULL;
    F = calloc(m * n, sizeof(int));
    int* Fcolors = calloc(m * n, sizeof(int));

    int diag = 0;
    int left = 0;
    int up = 0;

    // Setting F borders
    for (int i = 0; i < m; i++)
        F[i] = gap * i;
    for (int j = 0; j < n; j++)
        F[j * m] = gap * j;

    int max_l, i, j;
    for (int k = 1; k < m + n - 2; k++) {
        if (k > (m - 1)) {
            max_l = n - (k - (m - 1)) - 1;
            if (max_l > m - 1) max_l = m - 1;
        }
        else max_l = k;

#pragma omp parallel for schedule(guided)
        for (int l = 1; l <= max_l; l++) {
            if (k > (m - 1)) {
                i = k - (m - 1) + l;
                j = (m - 1) - l + 1;
            }
            else {
                i = l;
                j = k - i + 1;
            }
            // printf("\n==============================================\n");
            // printf("i : %d, j : %d\n", i, j);
            // printf("k : %d, l : %d, max_l : %d\n", k, l, max_l);
            // if (k > m) return;
            // F[i * m + j] = counter;
            // counter++;
            // print_sim_mat(F, A, B, m, n, i, j);
            diag = F[(i - 1) * m + j - 1] + (A[j - 1] == B[i - 1] ? match : mismatch);
            up = F[(i - 1) * m + j] + gap;
            left = F[i * m + j - 1] + gap;
            int var = MAX(MAX(diag, left), up);
            F[i * m + j] = var;
            Fcolors[i * m + j] = colors;
            colors--;
            // printf(" ");
        }
    }
    print_sim_mat(F, Fcolors, A, B, m, n, n - 1, m - 1);
    return F;
}


int main() {
    char A [] = "GCATGC";
    char B [] = "GATTA";

    int m = strlen(A) + 1;
    int n = strlen(B) + 1;

    int match = 1;
    int mismatch = -1;
    int gap = -1;

    unsigned long long before, after;
    double elapsed;

    before = rdtsc();
    int* F = diag_calculate_scoring_matrix(A, B, match, mismatch, gap);
    after = rdtsc();

    int score = align(F, A, B, match, mismatch, gap, 1);
    // int score = F[m * n - 1];

    // print_sim_mat(F, A, B, m, n, n - 1, m - 1);
    elapsed = (double)(after - before);
    printf("*Max score : %d\n", score);
    printf("*Cycles count : %.0f\n", elapsed);
    printf("*Time : %.3f sec\n", elapsed / 3.3e9);
    free(F);
}