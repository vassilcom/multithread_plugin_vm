
#pragma once


#include "my_item.h"





class __attribute__ ((visibility("hidden"))) plugin_handler {
public:
    plugin_handler() {}
    void load_plugin(const std::string &src, const std::string &name);
    void remove_plugins();

    void async_run();


    ~plugin_handler();


	
	//bool error = false;

	void stop_thread_loop();

	std::vector<my_item*> plugins;
private:
    py::scoped_interpreter interp;

    std::unique_ptr<py::gil_scoped_release> nogil;
    //std::atomic_bool keep_going;
};