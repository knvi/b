# b

![image](screenshot.png)

this project is originated from knicked's [CCraft](https://github.com/knicked/CCraft)

# WARNING
glfw is used as a submodule here, please run `git submodule update --init` after cloning to get glfw.

to build on linux/mac
```
(if youre running on WSL with x11) sudo apt-get install libx11-dev
mkdir build && cd build
cmake ..
make
```
win:
```
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
make
```
run
```
./b
```
