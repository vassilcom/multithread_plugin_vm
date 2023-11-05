import requests

def func(instance):
    while instance.keep_going:
        r = requests.get("http://en.cppreference.com/w/")