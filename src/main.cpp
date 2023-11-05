#include <atomic>
#include <memory>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <pybind11/embed.h>

namespace py = pybind11;
using namespace std::chrono_literals;

class __attribute__ ((visibility("hidden"))) plugin_handler {
public:
    plugin_handler() : keep_going(true) {}
    void load_plugins()
    {
        plugins.push_back(py::module::import("script1"));
        plugins.push_back(py::module::import("script2"));

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
    plugin_handler ph;
    ph.load_plugins();
    ph.async_run();

    std::this_thread::sleep_for(1s);

    return 0;
}