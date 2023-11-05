
#pragma once

#include <vector>

typedef struct {
	unsigned char* flttend3D;
	int rows;
	int cols;
    int chan;
} Matrix3D;


Matrix3D* matrix3D_create(int row, int col, int chan);
void matrix3D_free(Matrix3D *m);


