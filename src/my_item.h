
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
 


#include <iostream>


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


    //bool get_stop_token() { return keep_going.load();   }
	bool get_var_bool() { return var_bool; }
	int get_var_int_1() { return var_int_1; }
	int get_var_int_2() { return var_int_2;}
	void set_var_int_1(int v) { var_int_1 = v;}
	void set_var_int_2(int v) { var_int_2 = v;}
	void copyFloat(float x);	


	bool do_thread_loop = true;

private:

	py::module import_module_from_string();
	void replace_module_with_script(const std::string & script);

	py::module mod;
	std::thread * thr;
 

	bool var_bool=true;
	int var_int_1=50;
	int var_int_2=50;
	bool error = false;

	std::string m_src;
	std::string m_name;
};