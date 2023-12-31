#include "interface.h"


PYBIND11_EMBEDDED_MODULE(pybindings, m)
{
    py::class_<my_item>(m, "bindings")
	//.def_property_readonly("keep_going", &my_item::get_stop_token)
	.def_property_readonly("var_bool", &my_item::get_var_bool)
	.def_property_readonly("var_int_1", &my_item::get_var_int_1)
	.def_property_readonly("var_int_2", &my_item::get_var_int_2)
	.def("set_var_int_1", &my_item::set_var_int_1)
	.def("set_var_int_2", &my_item::set_var_int_2)
	.def("copyFloat",&my_item::copyFloat);
}

void plugin_handler::load_plugin(const std::string &src, const std::string &name)
{
	my_item* mc = new my_item(src,name);
	plugins.push_back(mc);		
	
    //plugins.push_back(py::module::import("script1"));
    //plugins.push_back(py::module::import("script2"));
    if (plugins.empty())
        throw std::runtime_error("no plugins loaded");
    std::cout << "plugins loaded.\n";
}

void plugin_handler::remove_plugins()
{
	for (auto & pl : plugins)
	{
		py::gil_scoped_acquire acquire;
		delete pl;
	}
	plugins.clear();
}

plugin_handler::~plugin_handler()
{
    std::cout << "destructing...\n";
	
	for (auto &mm : plugins) 
	{
		mm->remove_theard();
	}
	remove_plugins();
        
	
}

void plugin_handler::async_run()
{
	// trigger loop for every plugin tread
    for (const auto &mm : plugins) {
		mm->start_theard();
    }
    
    /*
     * We have called all Python code we need to; release GIL.
     * Is there some py::release_gil() instead of an object?
     */
    this->nogil = std::make_unique<py::gil_scoped_release>();
}

void plugin_handler::stop_thread_loop()
{
	for (const auto &mm : plugins) 
	{
		mm->do_thread_loop = false;
	}
}

// ___________________________

