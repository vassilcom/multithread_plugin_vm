#pragma once


namespace pybind11 { class scoped_interpreter; };

class t_mxw_python_interpreter_main
{
public:
	t_mxw_python_interpreter_main();
	~t_mxw_python_interpreter_main();
private:
	// python interpreter
	pybind11::scoped_interpreter* i = nullptr;
};

