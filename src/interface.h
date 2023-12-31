
#pragma once


#include "my_item.h"

#include "mxw_pyproxy_main.h"



class plugin_handler {
public:
    plugin_handler();
    void load_plugin(const std::string &src, const std::string &name);
    void remove_plugins();

    void async_run();


    ~plugin_handler();


	void stop_thread_loop();

	std::vector<my_item*> plugins;

	static t_mxw_python_interpreter_main* interpreter_py;
	
private:
    //py::scoped_interpreter interp;
	


    std::unique_ptr<py::gil_scoped_release> nogil;
    //std::atomic_bool keep_going;
};