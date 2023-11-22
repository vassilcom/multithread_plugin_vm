
# multithrad source ref: https://github.com/pybind/pybind11/issues/1360

# install python libs
	$ pip3 install opencv-python 
	$ pip3 install mediapipe
	$ apt-get install ffmpeg libsm6 libxext6 -y

# clone and run (on Linux)
	$ git clone --recurse-submodules -j8 https://github.com/vassilcom/multithread_plugin_vm.git
	$ code multithread_plugin_vm
		RUN AND DEBUG "my_build"