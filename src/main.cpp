#include "win.h"
#include <atomic>
#include <memory>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <pybind11/embed.h>


#include "interface.h"
#include "matrix.h"


namespace py = pybind11;
using namespace std::chrono_literals;

static bool do_thread_loop = true;
static Matrix3D* myVec3D2 = matrix3D_create(6, 6,4);
ImVec4 frame_bg = ImVec4(0, 0.2, 0.2,1);


bool imagedata_to_gpu(unsigned char* image_data,  GLuint* out_texture, int image_width, int image_height)
{
    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    

    *out_texture = image_texture;

	return true;
}


static char src1[1024 * 20] =R"(
import pybindings
import time
print("py src1 init")
def onFrame(scriptOp):
	#if scriptOp.keep_going:
	print("py scriptOp.keep_going")
	time.sleep(1.2)
	#r = requests.get("http://en.cppreference.com/w/")
)";

static char src2[1024 * 20] =R"(
import numpy as np
import cv2
import pybindings

# on re-exec aftr code edit previous capture neds to be released.
# vm_todo: how to get along without this ugly checkout?
if 'capture' in globals():
	capture.release()
capture = cv2.VideoCapture(0)

pic = np.random.randint(0, high=255, size=(6, 6, 4), dtype='uint8')

def onFrame(scriptOp):
	if(capture.isOpened()):
		ret, frame = capture.read()
		dim = (300, 300)
		frame = cv2.resize(frame, dim, interpolation=cv2.INTER_AREA)

		if scriptOp.var_bool:
			frame = cv2.Canny(frame, scriptOp.var_int_1, scriptOp.var_int_2)

		if(ret == True):
			frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
			frame = cv2.cvtColor(frame, cv2.COLOR_BGR2BGRA)
			scriptOp.vm_fun(frame)
		else:
			print('no frame available')
)";


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

	void replace_module_with_script2(const char* script, size_t index) {
		py::gil_scoped_acquire acquire;  // Acquire the GIL
		if (index < plugins.size())
		{
			try 
			{
				frame_bg = ImVec4(0, 0.2, 0.2, 1.0);
				py::exec(script, plugins[index].attr("__dict__"));
			} 
			catch (const py::error_already_set& e) 
			{
				frame_bg = ImVec4(0.2, 0.0, 0.0, 1.0);
				PyErr_Print();
			}
		}
	}

    void async_run()
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

    void callback(int data)
    {
        i_out = data;
    }

    bool get_stop_token() {
	    return keep_going.load();
    }

	bool get_var_bool() { return var_bool; }
	int get_var_int_1() { return var_int_1; }
	int get_var_int_2() { return var_int_2;}

    ~plugin_handler()
    {
        std::cout << "destructing...\n";
		keep_going.store(false);
        for (auto &t : threads)
            t.join();
    }

	int i_out;
	int i_in;
	bool var_bool=false;
	int var_int_1=50;
	int var_int_2=50;

	void vm_fun(pybind11::array_t<double> x)
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
					myVec3D->entries[i][j][k]= r(i, j, k);
					myVec3D->flttend3D[n] = (int)r(i, j, k);
					n++;
				}	

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
	.def_property_readonly("var_bool", &plugin_handler::get_var_bool)
	.def_property_readonly("var_int_1", &plugin_handler::get_var_int_1)
	.def_property_readonly("var_int_2", &plugin_handler::get_var_int_2)
    .def("callback", &plugin_handler::callback)
	.def("vm_fun",&plugin_handler::vm_fun);
}

int main()
{
    win my_win;
    my_win.init();

    plugin_handler ph;
    ph.load_plugins();
    ph.async_run();


	if (myVec3D2 && myVec3D2->is)
		std::cout << " myVec3D2->is \n";
	else
		std::cout << " NO myVec3D2->is \n";

	if (myVec3D && myVec3D->is)
		std::cout << " myVec3D->is \n";
	else
		std::cout << " NO myVec3D->is \n";

    GLuint my_image_texture = 0;
    imagedata_to_gpu(myVec3D2->flttend3D, &my_image_texture, myVec3D2->cols, myVec3D2->rows);

    while (my_win.loop())
    {


			if (myVec3D && myVec3D->is)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myVec3D->cols, myVec3D->rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, myVec3D->flttend3D);
			else
				std::cout << " NO myVec3D->is \n";

		
        my_win.pre_render();


        bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);

	    ImGui::SetNextWindowPos({ 20, 350}, ImGuiCond_FirstUseEver);
	    ImGui::SetNextWindowSize({ 700,350 }, ImGuiCond_FirstUseEver);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_bg);
        if(ImGui::Begin("python loop code"))
        {

			ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

			ImGui::Checkbox("var_bool", & ph.var_bool);
			ImGui::SliderInt("var_int_1",&ph.var_int_1, 1,600);
			ImGui::SliderInt("var_int_2",&ph.var_int_2, 1,600);

            if(ImGui::InputTextMultiline("##source", src2, IM_ARRAYSIZE(src2), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 25),ImGuiInputTextFlags_AllowTabInput))
            {
				//py_rebuild();
				ph.replace_module_with_script2(src2,1);
            }
        }
        ImGui::End();
        ImGui::PopStyleColor();

        // render frame buffer output image
	    ImGui::SetNextWindowPos({ 10,10 }, ImGuiCond_FirstUseEver);
	    ImGui::SetNextWindowSize({ 320,320 }, ImGuiCond_FirstUseEver);
        if(ImGui::Begin("image output"))
        {
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 size = ImGui::GetWindowSize();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddImage((void*)(intptr_t)my_image_texture,
                pos,
                ImVec2(pos.x + size.x, pos.y + size.y),
                ImVec2(0, 0),
                ImVec2(1, 1));
        }
        ImGui::End();

        my_win.imgui_render();
        my_win.opengl_render();
        my_win.swap_buffers();
	}
	do_thread_loop = false;

    return 0;
}