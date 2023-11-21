
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

#include "texture.h"

namespace py = pybind11;
using namespace std::chrono_literals;


class my_item
{
public:
	my_item(const std::string & script, const std::string & name);
	~my_item();
	void start_theard();
	void remove_theard();
	void render(int tex_id);

	py::module import_module_from_string();
	void replace_module_with_script(const std::string & script);

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

	std::string m_src;
	std::string m_name;


	texture m_texture;
private:


	
	//std::atomic_bool keep_going;
};