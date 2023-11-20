#include "interface.h"
#include "win.h"

PYBIND11_EMBEDDED_MODULE(pybindings, m)
{
    py::class_<my_class>(m, "bindings")
	//.def_property_readonly("keep_going", &my_class::get_stop_token)
	.def_property_readonly("var_bool", &my_class::get_var_bool)
	.def_property_readonly("var_int_1", &my_class::get_var_int_1)
	.def_property_readonly("var_int_2", &my_class::get_var_int_2)
	.def("copy3DNumpyArray",&my_class::copy3DNumpyArray);
}

void plugin_handler::load_plugin(const std::string &src, const std::string &name)
{
	my_class* mc = new my_class(src,name);
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

my_class::my_class(const std::string & script, const std::string & name): m_src(script), m_name(name)
{
	mod = import_module_from_string();
}

my_class::~my_class()
{
	matrix3D_free(myVec3D);
}

void my_class::start_theard()
{
	pybind11::module &m = mod;
	thr = new std::thread([&m, this]() 
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
void my_class::replace_module_with_script(const std::string & script) 
{
	py::gil_scoped_acquire acquire;  // Acquire the GIL
	
	{
		try 
		{
			error = false;
			py::exec(script, mod.attr("__dict__"));
		} 
		catch (const py::error_already_set& e) 
		{
			error = true;
			PyErr_Print();
		}
	}
}
py::module my_class::import_module_from_string() {
	py::gil_scoped_acquire acquire;  // Acquire the GIL
	// Create a Python module from the script
	PyObject* module = PyImport_AddModule(m_name.c_str());
	PyObject* dict = PyModule_GetDict(module);
	PyRun_String(m_src.c_str(), Py_file_input, dict, dict);
	// Import the temporary module into a Pybind11 module
	py::module pyModule = py::reinterpret_borrow<py::module>(module);
	// Clean up the temporary module
	Py_DECREF(module);
	return pyModule;
}


void my_class::remove_theard()
{
	thr->join();
	delete thr;
}

void my_class::render()
{
	std::string titl = m_name;

    if(ImGui::Begin(m_name.c_str()))
    {

		ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		ImGui::Checkbox("var_bool", & var_bool);
		ImGui::SliderInt("var_int_1",& var_int_1, 1,600);
		ImGui::SliderInt("var_int_2",& var_int_2, 1,600);

		if(error)
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2, 0.0, 0.0, 1.0));
		else
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0.0, 0.0, 0.5));
		
		
        if(ImGui::InputTextMultiline("##source", &m_src, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 30),ImGuiInputTextFlags_AllowTabInput))
        {
			//py_rebuild();
			replace_module_with_script(m_src);
        }

		ImGui::PopStyleColor();
    }
    ImGui::End();

}

void my_class::copy3DNumpyArray(pybind11::array_t<double> x)
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
