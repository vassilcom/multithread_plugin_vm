#include "win.h"
#include "interface.h"
#include "matrix.h"



std::string  src2=R"(
import pybindings
import time
import numpy as np
print("py src1 init")
def onFrame(scriptOp):
	pic = np.random.randint(0, high=255, size=(scriptOp.var_int_1, scriptOp.var_int_2, 4), dtype='uint8')
	scriptOp.copy3DNumpyArray(pic)
	time.sleep(0.1)
	print("hallo from py 1")
)";

std::string src1 =R"(
import numpy as np
import cv2
import pybindings

# on re-exec aftr code edit previous capture neds to be released.
# vm_todo: how to get along without this ugly checkout?
if 'capture' in globals():
	capture.release()
capture = cv2.VideoCapture(0)

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
			scriptOp.copy3DNumpyArray(frame)
		else:
			print('no frame available')
)";



int main()
{
	int w_width = 1200;
	int w_height = 800;

    win my_win;
    my_win.init(w_width, w_height);

    plugin_handler ph;

    ph.load_plugin(src1,"plugin_1");
    ph.load_plugin(src2,"plugin_2");

    ph.async_run();


	ImVec2 it_s(w_width / ph.plugins.size(),w_height); 

    while (my_win.loop())
    {


        my_win.pre_render();


        // bool show_demo_window = true;
        // ImGui::ShowDemoWindow(&show_demo_window);


		int pos_x = 0;
		int n = 0;
		for (auto & plugin : ph.plugins)
		{
			ImGui::SetNextWindowPos({ pos_x, 0}, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(it_s, ImGuiCond_FirstUseEver);
			plugin->render(n);
			pos_x += it_s.x;
			n += 1;
		}


        my_win.imgui_render();
        my_win.opengl_render();
        my_win.swap_buffers();
	}

	ph.stop_thread_loop();

    return 0;
}