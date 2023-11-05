#include "interface.h"

// write embedded py module. sort of bridg between py and c++
PYBIND11_EMBEDDED_MODULE (embeddedmodule, m) //second arg: we recieve a module with wich we can expose c++ to python
{
    // give name to module
    m.doc () = "Embedded Module";

    m.def ("copy2DNumpyArray", &copy2DNumpyArray);
    m.def ("copy3DNumpyArray", &copy3DNumpyArray);
}

void copy2DNumpyArray(pybind11::array_t<double> x) 
{
    auto r = x.unchecked<2>(); // Will throw if ndim != 3 or flags.writeable is false
    if(myVec2D == nullptr)
        myVec2D = matrix2D_create(r.shape(0), r.shape(1));
    for (pybind11::ssize_t i = 0; i < r.shape(0); i++)
        for (pybind11::ssize_t j = 0; j < r.shape(1); j++)
            myVec2D->entries[i][j] = r(i, j);
}

void copy3DNumpyArray(pybind11::array_t<double> x)
{
    int n = 0;
    auto r = x.unchecked<3>(); // x must have ndim = 3; can be non-writeable
    if(myVec3D == nullptr)
        myVec3D = matrix3D_create(r.shape(0), r.shape(1),r.shape(2)); // row col chan
    else if(myVec3D->rows != r.shape(0) || myVec3D->cols != r.shape(1))
    {
        matrix3D_free(myVec3D);
        myVec3D = matrix3D_create(r.shape(0), r.shape(1),r.shape(2)); // row col chan
    }
    
    for (pybind11::ssize_t i = 0; i < r.shape(0); i++)
        for (pybind11::ssize_t j = 0; j < r.shape(1); j++)
            for (pybind11::ssize_t k = 0; k < r.shape(2); k++)
            {
                myVec3D->entries[i][j][k]= r(i, j, k);
                myVec3D->flttend3D[n] = (int)r(i, j, k);
                n++;
            }

}