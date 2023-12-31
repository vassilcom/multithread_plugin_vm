
#include "matrix.h"


#include <stdlib.h>
#include <iostream>


Matrix3D* matrix3D_create(float _v){
	Matrix3D *matrix = (Matrix3D*)malloc(sizeof(Matrix3D));
	matrix->v = _v;
	return matrix;
}

void matrix3D_free(Matrix3D *m) {

	free(m);
	
	m = NULL;
}
