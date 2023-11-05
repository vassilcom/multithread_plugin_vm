
#include "matrix.h"


#include <stdlib.h>
#include <iostream>

Matrix2D* matrix2D_create(int row, int col) {
	Matrix2D *matrix = (Matrix2D*)malloc(sizeof(Matrix2D));
	matrix->rows = row;
	matrix->cols = col;
	matrix->entries = (double**)malloc(row * sizeof(double*));
	for (int i = 0; i < row; i++) {
		matrix->entries[i] = (double*)malloc(col * sizeof(double));
	}
	return matrix;
}
Matrix3D* matrix3D_create(int row, int col, int chan){
	Matrix3D *matrix = (Matrix3D*)malloc(sizeof(Matrix3D));
	matrix->rows = row;
	matrix->cols = col;
	matrix->chan = chan;
	matrix->flttend3D = (unsigned char*)malloc(chan * row* col* sizeof(unsigned char));
	matrix->entries = (double***)malloc(row * sizeof(double**));
	for (int i = 0; i < row; i++) {
		matrix->entries[i] = (double**)malloc(col * sizeof(double*));
        for (int k = 0; k < col; k++) {
            matrix->entries[i][k] = (double*)malloc(chan * sizeof(double));
        }
	}
	matrix->is = true;
	return matrix;
}

void matrix2D_free(Matrix2D *m) {
	for (int i = 0; i < m->rows; i++) {
		free(m->entries[i]);
	}
	free(m->entries);
	free(m);
	m = NULL;
}
void matrix3D_free(Matrix3D *m) {
	for (int i = 0; i < m->rows; i++) 
    {
        for (int j = 0; j < m->cols; j++) 
            free(m->entries[i][j]);  
        free(m->entries[i]);      
    }

	m->is = false;
	free(m->entries);
	free(m->flttend3D);
	free(m);
	
	m = NULL;
}
void matrix2D_print(Matrix2D* m) {
	printf("Rows: %d Columns: %d\n", m->rows, m->cols);
	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->cols; j++) {
			printf("%1.3f ", m->entries[i][j]);
		}
		printf("\n");
	}
}
void matrix3D_print(Matrix3D* m) {
	printf("Rows: %d Columns: %d channls: %d\n", m->rows, m->cols,m->chan);
	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->cols; j++) {
		    for (int k = 0; k < m->chan; k++) {
			    printf("%1.3f ", m->entries[i][j][k]);
            }
            printf(", ");
		}
		printf("\n");
	}
	printf("3D flttend3D:\n");
	for (int i = 0; i < m->rows * m->cols * m->chan; i++) {
		printf("%d , ", m->flttend3D[i]);
	}
	printf("\n");
}