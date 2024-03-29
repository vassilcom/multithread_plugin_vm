
#include "my_item.h"

#include "win.h"

my_item::my_item(const std::string & script, const std::string & name): m_src(script), m_name(name)
{
	mod = import_module_from_string();
}

my_item::~my_item()
{
	matrix3D_free(myVec3D);
}

void my_item::start_theard()
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
void my_item::replace_module_with_script(const std::string & script) 
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
py::module my_item::import_module_from_string() {
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


void my_item::remove_theard()
{
	thr->join();
	delete thr;
}

void my_item::render(int tex_id)
{
	if (myVec3D)
	{
		m_texture.Bind(tex_id);
		m_texture.data_to_gpu(myVec3D);
		m_texture.Unbind();
	}
	else
		std::cout << " NO myVec3D->is \n";


	std::string titl = m_name;

    if(ImGui::Begin(m_name.c_str()))
    {
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddImage((void*)(intptr_t)m_texture.m_RendererID,
            pos,
            ImVec2(pos.x + size.x, pos.y + size.x*0.7),
            ImVec2(0, 0),
            ImVec2(1, 1));

		ImGui::SetCursorPosY(size.x*0.7);


		ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		ImGui::Checkbox("var_bool", & var_bool);
		ImGui::SliderInt("var_int_1",& var_int_1, 1,600);
		ImGui::SliderInt("var_int_2",& var_int_2, 1,600);

		if(error)
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2, 0.0, 0.0, 1.0));
		else
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0.0, 0.0, 0.5));
		
		
        if(ImGui::InputTextMultiline("##source", &m_src, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 20),ImGuiInputTextFlags_AllowTabInput))
        {
			//py_rebuild();
			replace_module_with_script(m_src);
        }

		ImGui::PopStyleColor();
    }
    ImGui::End();

}

void my_item::copy3DNumpyArray(pybind11::array_t<double> x)
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
