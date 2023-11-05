#include "win.h"
#include <atomic>
#include <memory>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <pybind11/embed.h>

namespace py = pybind11;
using namespace std::chrono_literals;

static char src1[1024 * 20] =R"(
import requests
print("py src1 init")
def func(instance):
	print("py src1 func")
	while instance.keep_going:
		r = requests.get("http://en.cppreference.com/w/")
)";

static char src2[1024 * 20] =R"(
import pybindings
import time
print("py src2 init")
def func(instance):
	print("py src2 func")

	while instance.keep_going:
		time.sleep(0.2)
		instance.callback(42)
)";

void replace_module_with_script(std::vector<py::module>& moduleVector, const char* script, size_t index) {
    py::gil_scoped_acquire acquire;  // Acquire the GIL

    if (index < moduleVector.size()) {
        // Create a new Python module from the script
        PyObject* module = PyImport_AddModule("__temp_module__");
        PyObject* dict = PyModule_GetDict(module);
        PyRun_String(script, Py_file_input, dict, dict);

        // Import the temporary module into a Pybind11 module
        py::module newModule = py::reinterpret_borrow<py::module>(module);

        // Replace the existing module in the vector
        moduleVector[index] = newModule;

        // Clean up the temporary module
        Py_DECREF(module);
    }
}
py::module import_module_from_string(const char* script, const char* name) {
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

// altrnative?
py::module import_module_from_string2(const char* script) {
    py::gil_scoped_acquire acquire;  // Acquire the GIL
    // Create a Python module from the script
    py::module pyModule = py::module::import("__main__");
	py::exec(script, pyModule.attr("__dict__"));
	return pyModule;
}
void replace_module_with_script2(std::vector<py::module>& moduleVector, const char* script, size_t index) {
    py::gil_scoped_acquire acquire;  // Acquire the GIL
    if (index < moduleVector.size())
		py::exec(script, moduleVector[index].attr("__dict__"));
}

class __attribute__ ((visibility("hidden"))) plugin_handler {
public:
    plugin_handler() : keep_going(true) {}
    void load_plugins()
    {
        plugins.push_back(import_module_from_string(src1,"sr1"));
        plugins.push_back(import_module_from_string(src2,"sr2"));
        //plugins.push_back(py::module::import("script1"));
        //plugins.push_back(py::module::import("script2"));
        if (plugins.empty())
            throw std::runtime_error("no plugins loaded");

        std::cout << "plugins loaded.\n";
    }

    void async_run()
    {
        for (int i = 0; i < 10; i++) {
            for (const auto &m : plugins) {
                threads.emplace_back([&m, this]() {
                    /* Required whenever we need to run anything Python. */
                    py::gil_scoped_acquire gil;

                    m.attr("func")(this);
                });
            }
        }

        /*
         * We have called all Python code we need to; release GIL.
         * Is there some py::release_gil() instead of an object?
         */
        this->nogil = std::make_unique<py::gil_scoped_release>();
    }

    void callback(int data)
    {
        std::ignore = data;
    }

    bool get_stop_token() {
	    return keep_going.load();
    }
    ~plugin_handler()
    {
        std::cout << "destructing...\n";
	keep_going.store(false);
        for (auto &t : threads)
            t.join();
    }

private:
    py::scoped_interpreter interp;
    std::vector<std::thread> threads;
    std::vector<py::module> plugins;
    std::unique_ptr<py::gil_scoped_release> nogil;
    std::atomic_bool keep_going;
};


PYBIND11_EMBEDDED_MODULE(pybindings, m)
{
    py::class_<plugin_handler>(m, "bindings")
	.def_property_readonly("keep_going", &plugin_handler::get_stop_token)
        .def("callback", &plugin_handler::callback);
}

int main()
{
    win my_win;
    my_win.init();

    plugin_handler ph;
    ph.load_plugins();
    ph.async_run();

    while (my_win.loop())
    {
        my_win.pre_render();


        bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);

	    ImGui::SetNextWindowPos({ 20, 350}, ImGuiCond_FirstUseEver);
	    ImGui::SetNextWindowSize({ 700,350 }, ImGuiCond_FirstUseEver);
        //ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg);
        if(ImGui::Begin("python loop code"))
        {
			ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        }
        ImGui::End();
        //ImGui::PopStyleColor();


        my_win.imgui_render();
        my_win.opengl_render();
        my_win.swap_buffers();
	}

    return 0;
}