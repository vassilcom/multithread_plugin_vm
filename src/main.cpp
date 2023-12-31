#include "win.h"
#include "interface.h"
 



std::string  src2=R"(
import pybindings
print("py src1 init")
def onFrame(scriptOp):
	print("py src1 init")
)";

std::string src1 =R"(
import pybindings
def onFrame(scriptOp):
	print("py src1 init")
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