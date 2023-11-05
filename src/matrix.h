
#pragma once

#include <vector>

typedef struct {
	double** entries;
	int rows;
	int cols;
} Matrix2D;

typedef struct {
	double*** entries;
	unsigned char* flttend3D;
	int rows;
	int cols;
    int chan;
	bool is = false;
} Matrix3D;


Matrix2D* matrix2D_create(int row, int col);
Matrix3D* matrix3D_create(int row, int col, int chan);
void matrix2D_free(Matrix2D *m);
void matrix3D_free(Matrix3D *m);
void matrix2D_print(Matrix2D* m);
void matrix3D_print(Matrix3D* m);


