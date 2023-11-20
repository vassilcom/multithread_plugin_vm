#include "win.h"
#include "interface.h"
#include "matrix.h"




void imagedata_to_gpu(unsigned char* image_data,  GLuint* out_texture, int image_width, int image_height)
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
}


std::string  src2=R"(
import pybindings
import time
print("py src1 init")
def onFrame(scriptOp):
	time.sleep(0.5)
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
			scriptOp.copy3DNumpyArray(frame)
		else:
			print('no frame available')
)";



int main()
{
    win my_win;
    my_win.init();

    plugin_handler ph;

    ph.load_plugin(src1,"src1");
    ph.load_plugin(src2,"src2");

    ph.async_run();

    GLuint my_image_texture = 0;
    
	if (ph.plugins[0]->myVec3D)
		imagedata_to_gpu(ph.plugins[0]->myVec3D->flttend3D, &my_image_texture, ph.plugins[0]->myVec3D->cols, ph.plugins[0]->myVec3D->rows);
	else
	{
		Matrix3D* myVec3D2 = matrix3D_create(6, 6,4);
		imagedata_to_gpu(myVec3D2->flttend3D, &my_image_texture, myVec3D2->cols, myVec3D2->rows);		
	}


    while (my_win.loop())
    {
		if (ph.plugins[0]->myVec3D)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ph.plugins[0]->myVec3D->cols, ph.plugins[0]->myVec3D->rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, ph.plugins[0]->myVec3D->flttend3D);
		else
			std::cout << " NO myVec3D->is \n";

		
        my_win.pre_render();


        // bool show_demo_window = true;
        // ImGui::ShowDemoWindow(&show_demo_window);

	    ImGui::SetNextWindowPos({ 520, 10}, ImGuiCond_FirstUseEver);
	    ImGui::SetNextWindowSize({ 748,532 }, ImGuiCond_FirstUseEver);
		ph.plugins[0]->render();


        

		ImGui::SetNextWindowPos({ 14,476}, ImGuiCond_FirstUseEver);
	    ImGui::SetNextWindowSize({ 338,226}, ImGuiCond_FirstUseEver);
		ph.plugins[1]->render();




        // render frame buffer output image
	    ImGui::SetNextWindowPos({ 10,10 }, ImGuiCond_FirstUseEver);
	    ImGui::SetNextWindowSize({ 500,400 }, ImGuiCond_FirstUseEver);
        if(ImGui::Begin("plugin 1 output"))
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

	ph.stop_thread_loop();

    return 0;
}