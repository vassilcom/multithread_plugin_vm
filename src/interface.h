
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


class my_class
{
public:
	my_class(const py::module vv);
	~my_class();
	void start_theard();
	void remove_theard();
	void render();

	void copy3DNumpyArray(pybind11::array_t<double> x);
    //bool get_stop_token() { return keep_going.load();   }
	bool get_var_bool() { return var_bool; }
	int get_var_int_1() { return var_int_1; }
	int get_var_int_2() { return var_int_2;}
	
	py::module mod;
	std::thread * thr;
	Matrix3D* myVec3D = nullptr;
	bool do_thread_loop = true;

	int i_out;
	int i_in;
	bool var_bool=false;
	int var_int_1=50;
	int var_int_2=50;
	bool error = false;
private:


	
	//std::atomic_bool keep_going;
};

class __attribute__ ((visibility("hidden"))) plugin_handler {
public:
    plugin_handler() {}
    void load_plugins(const std::string &src1, const std::string &src2);
    void remove_plugins();

	py::module import_module_from_string(const std::string & script, const std::string & name);

	void replace_module_with_script2(const std::string & script, size_t index);

    void async_run();


    ~plugin_handler();


	
	//bool error = false;

	void stop_thread_loop();

	std::vector<my_class*> plugins;
private:
    py::scoped_interpreter interp;

    std::unique_ptr<py::gil_scoped_release> nogil;
    //std::atomic_bool keep_going;
};