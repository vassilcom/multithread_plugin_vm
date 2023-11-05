
#include "matrix.h"


#include <stdlib.h>
#include <iostream>


Matrix3D* matrix3D_create(int row, int col, int chan){
	Matrix3D *matrix = (Matrix3D*)malloc(sizeof(Matrix3D));
	matrix->rows = row;
	matrix->cols = col;
	matrix->chan = chan;
	matrix->flttend3D = (unsigned char*)malloc(chan * row* col* sizeof(unsigned char));
	
	return matrix;
}

void matrix3D_free(Matrix3D *m) {

	free(m->flttend3D);
	free(m);
	
	m = NULL;
}
