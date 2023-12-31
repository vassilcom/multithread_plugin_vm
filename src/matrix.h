
#pragma once

#include <vector>

typedef struct {
	float v;
} Matrix3D;


Matrix3D* matrix3D_create(float _v);
void matrix3D_free(Matrix3D *m);


