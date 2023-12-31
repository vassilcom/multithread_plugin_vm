

#include "mxw_pyproxy_main.h"
#include <pybind11/embed.h> // everything needed for embedding

t_mxw_python_interpreter_main::t_mxw_python_interpreter_main()
{
	i = new pybind11::scoped_interpreter();
}
t_mxw_python_interpreter_main::~t_mxw_python_interpreter_main()
{
	delete i;
	i = nullptr;
}

