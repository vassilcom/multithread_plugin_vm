
#pragma once

#include <atomic>
#include <memory>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "matrix.h"


#include <iostream>


namespace py = pybind11;
using namespace std::chrono_literals;




class __attribute__ ((visibility("hidden"))) plugin_handler {
public:
    plugin_handler() : keep_going(true) {}
    void load_plugins(const char* src1, const char* src2);

	py::module import_module_from_string(const char* script, const char* name);

	void replace_module_with_script2(const char* script, size_t index);

    void async_run();

    void callback(int data) {  i_out = data; }
	void copy3DNumpyArray(pybind11::array_t<double> x);

    bool get_stop_token() { return keep_going.load();   }
	bool get_var_bool() { return var_bool; }
	int get_var_int_1() { return var_int_1; }
	int get_var_int_2() { return var_int_2;}

    ~plugin_handler();

	int i_out;
	int i_in;
	bool var_bool=false;
	int var_int_1=50;
	int var_int_2=50;

	bool do_thread_loop = true;
	bool error = false;

	

	Matrix3D* myVec3D = nullptr;

private:
    py::scoped_interpreter interp;
    std::vector<std::thread> threads;
    std::vector<py::module> plugins;
    std::unique_ptr<py::gil_scoped_release> nogil;
    std::atomic_bool keep_going;
};