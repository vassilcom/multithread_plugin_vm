#include "interface.h"

PYBIND11_EMBEDDED_MODULE(pybindings, m)
{
    py::class_<plugin_handler>(m, "bindings")
	.def_property_readonly("keep_going", &plugin_handler::get_stop_token)
	.def_property_readonly("var_bool", &plugin_handler::get_var_bool)
	.def_property_readonly("var_int_1", &plugin_handler::get_var_int_1)
	.def_property_readonly("var_int_2", &plugin_handler::get_var_int_2)
    .def("callback", &plugin_handler::callback)
	.def("copy3DNumpyArray",&plugin_handler::copy3DNumpyArray);
}

void plugin_handler::load_plugins(const char* src1, const char* src2)
{
    plugins.push_back(import_module_from_string(src1,"sr1"));
    plugins.push_back(import_module_from_string(src2,"sr2"));
    //plugins.push_back(py::module::import("script1"));
    //plugins.push_back(py::module::import("script2"));
    if (plugins.empty())
        throw std::runtime_error("no plugins loaded");
    std::cout << "plugins loaded.\n";
}
plugin_handler::~plugin_handler()
{
    std::cout << "destructing...\n";
	keep_going.store(false);
    for (auto &t : threads)
        t.join();
	matrix3D_free(myVec3D);
}
py::module plugin_handler::import_module_from_string(const char* script, const char* name) {
	py::gil_scoped_acquire acquire;  // Acquire the GIL
	// Create a Python module from the script
	PyObject* module = PyImport_AddModule(name);
	PyObject* dict = PyModule_GetDict(module);
	PyRun_String(script, Py_file_input, dict, dict);
	// Import the temporary module into a Pybind11 module
	py::module pyModule = py::reinterpret_borrow<py::module>(module);
	// Clean up the temporary module
	Py_DECREF(module);
	return pyModule;
}

void plugin_handler::replace_module_with_script2(const char* script, size_t index) 
{
	py::gil_scoped_acquire acquire;  // Acquire the GIL
	if (index < plugins.size())
	{
		try 
		{
			error = false;
			py::exec(script, plugins[index].attr("__dict__"));
		} 
		catch (const py::error_already_set& e) 
		{
			error = true;
			PyErr_Print();
		}
	}
}

void plugin_handler::async_run()
{
	// trigger loop for every plugin tread
    for (const auto &m : plugins) {
        threads.emplace_back([&m, this]() 
		{
			while (do_thread_loop) 
			{
				/* Required whenever we need to run anything Python. */
				py::gil_scoped_acquire gil;
				try 
				{
					m.attr("onFrame")(this);
				} 
				catch (const py::error_already_set& e) 
				{
					PyErr_Print();
				}
			}
        });
    }
    
    /*
     * We have called all Python code we need to; release GIL.
     * Is there some py::release_gil() instead of an object?
     */
    this->nogil = std::make_unique<py::gil_scoped_release>();
}

void plugin_handler::copy3DNumpyArray(pybind11::array_t<double> x)
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
				myVec3D->flttend3D[n] = (int)r(i, j, k);
				n++;
			}	
}