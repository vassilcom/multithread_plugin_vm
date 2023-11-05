import pybindings
import time

def func(instance):
    while instance.keep_going:
        time.sleep(0.2)
        instance.callback(42)